//---------------------------------------------------------------------------//
/*
  Copyright (c) 2012, Stuart R. Slattery
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  *: Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  *: Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  *: Neither the name of the University of Wisconsin - Madison nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//---------------------------------------------------------------------------//
/*!
 * \brief DTK_STKMeshEntityIntegrationRule.cpp
 * \author Stuart R. Slattery
 * \brief STK mesh integration rule implementation.
 */
//---------------------------------------------------------------------------//

#include "DTK_STKMeshEntityIntegrationRule.hpp"
#include "DTK_STKMeshHelpers.hpp"

#include <Intrepid_FieldContainer.hpp>

namespace DataTransferKit
{
//---------------------------------------------------------------------------//
// Constructor.
STKMeshEntityIntegrationRule::STKMeshEntityIntegrationRule(
    const Teuchos::RCP<stk::mesh::BulkData>& bulk_data )
    : d_bulk_data( bulk_data )
{ /* ... */ }

//---------------------------------------------------------------------------//
// Given an entity and an integration order, get its integration rule. 
void STKMeshEntityIntegrationRule::getIntegrationRule(
    const Entity& entity,
    const int order,
    Teuchos::Array<Teuchos::Array<double> >& reference_points,
    Teuchos::Array<double>& weights ) const override;
{
    // Get entity and topology info.
    const stk::mesh::Entity& stk_entity =
	STKMeshHelpers::extractEntity( entity );
    shards::CellTopology cell_topo =
	STKMeshHelpers::getShardsTopology( stk_entity );
    std::pair<unsigned,int> cub_key( cell_topo.getKey(), order );

    // If we haven't already created a cubature for this topology and order
    // create one.
    Teuchos::RCP<Intrepid::Cubature<double> > cub_rule;
    if ( d_cub_rules.count(cub_key) )
    {
	cub_rule = d_cub_rules.find( cub_key )->second;
    }
    else
    {
	cub_rule = d_intrepid_factory.create( cell_topo, order );
	d_cub_rules.emplace( cub_key, cub_rule );
    }

    // Get the cubature rule.
    int num_points = cub_rule->getNumPoints();
    int cub_dim = cub_rule->getDimension();
    Intrepid::FieldContainer<double> cub_points( num_points, cub_dim );
    Intrepid::FieldContainer<double> cub_weights( num_points );
    d_cub_rule->getCubature( cub_points, cub_weights );

    // Write the data into the output arrays.
    reference_points.resize( num_points );
    weights.resize( num_points );
    for ( int p = 0; p < num_points; ++p )
    {
	weights[p] = cub_weights(p);
	reference_points[p].resize( cub_dim );
	for ( int d = 0; d < cub_dim; ++d )
	{
	    reference_points[p][d] = cub_points(p,d);
	}
    }
}

//---------------------------------------------------------------------------//

} // end namespace DataTransferKit

//---------------------------------------------------------------------------//
// end DTK_STKMeshEntityIntegrationRule.hpp
//---------------------------------------------------------------------------//
