//---------------------------------------------------------------------------//
/*!
 * \file DTK_Rendezvous.hpp
 * \author Stuart R. Slattery
 * \brief Rendezous declaration.
 */
//---------------------------------------------------------------------------//

#ifndef DTK_RENDEZVOUS_HPP
#define DTK_RENDEZVOUS_HPP

#include <vector>
#include <set>

#include "DTK_RendezvousMesh.hpp"
#include "DTK_KDTree.hpp"
#include "DTK_RCB.hpp"
#include "DTK_BoundingBox.hpp"
#include "DTK_MeshTraits.hpp"

#include <Teuchos_RCP.hpp>
#include <Teuchos_Comm.hpp>
#include <Teuchos_Array.hpp>

#include <Tpetra_Map.hpp>

namespace DataTransferKit
{

template<class Mesh>
class Rendezvous
{
    
  public:

    //@{
    //! Typedefs.
    typedef Mesh                                 mesh_type;
    typedef MeshTraits<Mesh>                     MT;
    typedef typename MT::handle_type             handle_type;
    typedef RendezvousMesh<handle_type>          RendezvousMeshType;
    typedef Teuchos::RCP<RendezvousMeshType>     RCP_RendezvousMesh;
    typedef KDTree<handle_type>                  KDTreeType;
    typedef Teuchos::RCP<KDTreeType>             RCP_KDTree;
    typedef RCB<Mesh>                            RCBType;
    typedef Teuchos::RCP<RCBType>                RCP_RCB;
    typedef Teuchos::Comm<int>                   CommType;
    typedef Teuchos::RCP<const CommType>         RCP_Comm;
    typedef Tpetra::Map<handle_type>             TpetraMap;
    typedef Teuchos::RCP<const TpetraMap>        RCP_TpetraMap;
    typedef handle_type                          ordinal_type;
    //@}

    // Constructor.
    Rendezvous( const RCP_Comm& global_comm, const BoundingBox& global_box );

    // Destructor.
    ~Rendezvous();

    // Build the rendezvous decomposition.
    void build( const Mesh& mesh );

    // Get the rendezvous processes for a list of node coordinates.
    std::vector<int> 
    getRendezvousProcs( const std::vector<double> &coords ) const;

    // Get the native mesh elements containing a list of coordinates.
    std::vector<handle_type>
    getElements( const std::vector<double>& coords ) const;

    // Get the rendezvous mesh.
    const RCP_RendezvousMesh& getMesh() const
    { return d_rendezvous_mesh; }

  private:

    // Extract the mesh nodes and elements that are in a bounding box.
    void getMeshInBox( const Mesh& mesh,
		       const BoundingBox& box,
		       std::vector<char>& nodes_in_box,
		       std::vector<char>& elements_in_box );

    // Send the mesh to the rendezvous decomposition and build the concrete
    // mesh. 
    void sendMeshToRendezvous( const Mesh& mesh,
			       const std::vector<char>& elements_in_box );

    // Setup the communication patterns.
    void setupCommunication( const Mesh& mesh,
			     const std::vector<char>& elements_in_box,
			     Teuchos::Array<handle_type>& rendezvous_nodes,
			     Teuchos::Array<handle_type>& rendezvous_elements );

  private:

    // Global communicator over which to perform the rendezvous.
    RCP_Comm d_global_comm;

    // Bounding box in which to perform the rendezvous.
    BoundingBox d_global_box;

    // Rendezvous partitioning.
    RCP_RCB d_rcb;

    // Rendezvous on-process mesh.
    RCP_RendezvousMesh d_rendezvous_mesh;

    // Rendezvous on-process kD-tree.
    RCP_KDTree d_kdtree;
};

} // end namespace DataTransferKit

//---------------------------------------------------------------------------//
// Template includes.
//---------------------------------------------------------------------------//

#include "DTK_Rendezvous_def.hpp"

#endif // end DTK_RENDEZVOUS_HPP

//---------------------------------------------------------------------------//
// end DTK_Rendezvous.hpp
//---------------------------------------------------------------------------//
