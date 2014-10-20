//---------------------------------------------------------------------------//
/*
  Copyright (c) 2014, Stuart R. Slattery
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  *: Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  *: Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  *: Neither the name of the Oak Ridge National Laboratory nor the
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
 * \brief DTK_EntityLocalMap.cpp
 * \author Stuart R. Slattery
 * \brief Forward and reverse local mappings for entities.
 */
//---------------------------------------------------------------------------//

#include <cmath>
#include <limits>

#include "DTK_EntityLocalMap.hpp"
#include "DTK_DBC.hpp"

namespace DataTransferKit
{
//---------------------------------------------------------------------------//
// Constructor.
EntityLocalMap::EntityLocalMap()
{ /* ... */ }

//---------------------------------------------------------------------------//
// Destructor.
EntityLocalMap::~EntityLocalMap()
{ /* ... */ }

//---------------------------------------------------------------------------//
// Set parameters for mapping.
void EntityLocalMap::setParameters( 
    const Teuchos::RCP<Teuchos::ParameterList>& parameters )
{
    b_parameters = parameters;
}

//---------------------------------------------------------------------------//
// Return the entity measure with respect to the parameteric dimension (volume
// for a 3D entity, area for 2D, and length for 1D). 
double EntityLocalMap::measure( const Entity& entity ) const
{
    bool not_implemented = true;
    DTK_INSIST( !not_implemented );
    return -1.0;
}

//---------------------------------------------------------------------------//
// Return the centroid of the entity.
void EntityLocalMap::centroid( 
    const Entity& entity, const Teuchos::ArrayView<double>& centroid ) const
{ 
    bool not_implemented = true;
    DTK_INSIST( !not_implemented );
}

//---------------------------------------------------------------------------//
// Perform a safeguard check for mapping a point to the reference space
// of an entity using the given tolerance. 
bool EntityLocalMap::isSafeToMapToReferenceFrame(
    const Entity& entity,
    const Teuchos::ArrayView<const double>& point,
    const Teuchos::RCP<MappingStatus>& status ) const
{
    bool not_implemented = true;
    DTK_INSIST( !not_implemented );
    return false;
}

//---------------------------------------------------------------------------//
// Map a point to the reference space of an entity. Return the parameterized point.
bool EntityLocalMap::mapToReferenceFrame( 
    const Entity& entity,
    const Teuchos::ArrayView<const double>& point,
    const Teuchos::ArrayView<double>& reference_point,
    const Teuchos::RCP<MappingStatus>& status ) const
{
    bool not_implemented = true;
    DTK_INSIST( !not_implemented );
    return false;
}

//---------------------------------------------------------------------------//
// Determine if a reference point is in the parameterized space of an entity.
bool EntityLocalMap::checkPointInclusion( 
    const Entity& entity,
    const Teuchos::ArrayView<const double>& reference_point ) const
{
    bool not_implemented = true;
    DTK_INSIST( !not_implemented );
    return false;
}

//---------------------------------------------------------------------------//
// Map a reference point to the physical space of an entity.
void EntityLocalMap::mapToPhysicalFrame( 
    const Entity& entity,
    const Teuchos::ArrayView<const double>& reference_point,
    const Teuchos::ArrayView<double>& point ) const
{
    bool not_implemented = true;
    DTK_INSIST( !not_implemented );
}

//---------------------------------------------------------------------------//
// Compute the normal on a face (3D) or edge (2D) at a given reference point.
void EntityLocalMap::normalAtReferencePoint( 
    const Entity& entity,
    const Teuchos::ArrayView<double>& reference_point,
    const Teuchos::ArrayView<double>& normal ) const
{
    // Determine the reference dimension.
    int physical_dim = entity.physicalDimension();
    int ref_dim = physical_dim - 1;

    // Create a perturbation.
    double perturbation = 
	std::sqrt( std::numeric_limits<double>::epsilon() );

    // 3D/face case.
    if ( 2 == ref_dim )
    {
	DTK_CHECK( 3 == reference_point.size() );
	DTK_CHECK( 3 == normal.size() );

	// Create extra points.
	Teuchos::Array<double> ref_p1( reference_point );
	Teuchos::Array<double> ref_p2( reference_point );
    
	// Apply a perturbation to the extra points.
	double p1_sign = 1.0;
	ref_p1[0] += perturbation;
	if ( !this->checkPointInclusion(entity,ref_p1()) )
	{
	    ref_p1[0] -= 2*perturbation;
	    p1_sign = -1.0;
	}
	double p2_sign = 1.0;
	ref_p2[1] += perturbation;
	if ( !this->checkPointInclusion(entity,ref_p2()) )
	{
	    ref_p2[1] -= 2*perturbation;
	    p2_sign = -1.0;
	}

	// Map the perturbed points to the physical frame.
	Teuchos::Array<double> p0( physical_dim );
	this->mapToPhysicalFrame( entity, reference_point(), p0() );
	Teuchos::Array<double> p1( physical_dim );
	this->mapToPhysicalFrame( entity, ref_p1(), p1() );
	Teuchos::Array<double> p2( physical_dim );
	this->mapToPhysicalFrame( entity, ref_p2(), p2() );

	// Compute the cross product of the tangents produced by the
	// perturbation.
	Teuchos::Array<double> tan1( physical_dim );
	Teuchos::Array<double> tan2( physical_dim );
	for ( int d = 0; d < physical_dim; ++d )
	{
	    tan1[d] = p1_sign*(p1[d] - p0[d]);
	    tan2[d] = p2_sign*(p2[d] - p0[d]);
	}
	normal[0] = tan1[1]*tan2[2] - tan1[2]*tan2[1];
	normal[1] = tan1[2]*tan2[0] - tan1[0]*tan2[2];
	normal[2] = tan1[0]*tan2[1] - tan1[1]*tan2[0];
    } 

    // 2D/edge case.
    else if ( 1 == ref_dim )
    {
	DTK_CHECK( 2 == reference_point.size() );
	DTK_CHECK( 2 == normal.size() );

	// Create extra points.
	Teuchos::Array<double> ref_p1( reference_point );
    
	// Apply a perturbation to the extra points.
	double p1_sign = 1.0;
	ref_p1[0] += perturbation;
	if ( !this->checkPointInclusion(entity,ref_p1()) )
	{
	    ref_p1[0] -= 2*perturbation;
	    p1_sign = -1.0;
	}

	// Map the perturbed points to the physical frame.
	Teuchos::Array<double> p0( physical_dim );
	this->mapToPhysicalFrame( entity, reference_point(), p0() );
	Teuchos::Array<double> p1( physical_dim );
	this->mapToPhysicalFrame( entity, ref_p1(), p1() );

	// Compute the cross product of the tangents produced by the
	// perturbation.
	Teuchos::Array<double> tan( physical_dim );
	for ( int d = 0; d < physical_dim; ++d )
	{
	    tan[d] = p1_sign*(p1[d] - p0[d]);
	}
	normal[0] = -tan[0];
	normal[1] = tan[1];
    }

    // Normalize the normal vector.
    double norm = 0.0;
    for ( int d = 0; d < physical_dim; ++d )
    {
	norm += normal[d]*normal[d];
    }
    norm = std::sqrt(norm);
    for ( int d = 0; d < physical_dim; ++d )
    {
	normal[d] /= norm;
    }
}

//---------------------------------------------------------------------------//

} // end namespace DataTransferKit

//---------------------------------------------------------------------------//
// end DTK_EntityLocalMap.cpp
//---------------------------------------------------------------------------//
