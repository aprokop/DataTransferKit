/****************************************************************************
 * Copyright (c) 2012-2018 by the DataTransferKit authors                   *
 * All rights reserved.                                                     *
 *                                                                          *
 * This file is part of the DataTransferKit library. DataTransferKit is     *
 * distributed under a BSD 3-clause license. For the licensing terms see    *
 * the LICENSE file in the top-level directory.                             *
 *                                                                          *
 * SPDX-License-Identifier: BSD-3-Clause                                    *
 ****************************************************************************/

#ifndef DTK_LINEAR_BVH_DEF_HPP
#define DTK_LINEAR_BVH_DEF_HPP

#include "DTK_ConfigDefs.hpp"

#include <DTK_DetailsAlgorithms.hpp>
#include <DTK_DetailsTreeConstruction.hpp>
#include <DTK_KokkosHelpers.hpp>

#include <Kokkos_ArithTraits.hpp>

namespace DataTransferKit
{
template <typename DeviceType>
class SetBoundingBoxesFunctor
{
  public:
    using ExecutionSpace = typename DeviceType::execution_space;

    SetBoundingBoxesFunctor(
        Kokkos::View<Node *, DeviceType> leaf_nodes,
        Kokkos::View<int *, DeviceType> indices,
        Kokkos::View<Box const *, DeviceType> bounding_boxes )
        : _leaf_nodes( leaf_nodes )
        , _indices( indices )
        , _bounding_boxes( bounding_boxes )
    {
    }

    KOKKOS_INLINE_FUNCTION
    void operator()( int const i ) const
    {
        _leaf_nodes[i].bounding_box = _bounding_boxes[_indices[i]];
    }

  private:
    Kokkos::View<Node *, DeviceType> _leaf_nodes;
    Kokkos::View<int *, DeviceType> _indices;
    Kokkos::View<Box const *, DeviceType> _bounding_boxes;
};

template <typename DeviceType>
BVH<DeviceType>::BVH( Kokkos::View<Box const *, DeviceType> bounding_boxes )
    : _leaf_nodes( "leaf_nodes", bounding_boxes.extent( 0 ) )
    , _internal_nodes( "internal_nodes", bounding_boxes.extent( 0 ) > 0
                                             ? bounding_boxes.extent( 0 ) - 1
                                             : 0 )
    , _indices( "sorted_indices", bounding_boxes.extent( 0 ) )
{
    using ExecutionSpace = typename DeviceType::execution_space;

    if ( empty() )
    {
        return;
    }

    if ( size() == 1 )
    {
        iota( _indices );
        Kokkos::parallel_for( DTK_MARK_REGION( "set_bounding_boxes" ),
                              Kokkos::RangePolicy<ExecutionSpace>( 0, 1 ),
                              SetBoundingBoxesFunctor<DeviceType>(
                                  _leaf_nodes, _indices, bounding_boxes ) );
        Kokkos::fence();
        return;
    }

    // determine the bounding box of the scene
    Details::TreeConstruction<DeviceType>::calculateBoundingBoxOfTheScene(
        bounding_boxes, _internal_nodes[0].bounding_box );

    // calculate morton code of all objects
    int const n = bounding_boxes.extent( 0 );
    Kokkos::View<unsigned int *, DeviceType> morton_indices( "morton", n );
    Details::TreeConstruction<DeviceType>::assignMortonCodes(
        bounding_boxes, morton_indices, _internal_nodes[0].bounding_box );

    // sort them along the Z-order space-filling curve
    iota( _indices );
    Details::TreeConstruction<DeviceType>::sortObjects( morton_indices,
                                                        _indices );

    // generate bounding volume hierarchy
    Kokkos::parallel_for( DTK_MARK_REGION( "set_bounding_boxes" ),
                          Kokkos::RangePolicy<ExecutionSpace>( 0, n ),
                          SetBoundingBoxesFunctor<DeviceType>(
                              _leaf_nodes, _indices, bounding_boxes ) );
    Kokkos::fence();
    Details::TreeConstruction<DeviceType>::generateHierarchy(
        morton_indices, _leaf_nodes, _internal_nodes );

    // calculate bounding box for each internal node by walking the hierarchy
    // toward the root
    Details::TreeConstruction<DeviceType>::calculateBoundingBoxes(
        _leaf_nodes, _internal_nodes );
}

} // end namespace DataTransferKit

// Explicit instantiation macro
#define DTK_LINEAR_BVH_INSTANT( NODE )                                         \
    template class BVH<typename NODE::device_type>;

#endif
