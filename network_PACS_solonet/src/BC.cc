/*
 * FractureBCHandler.cc
 *
 *  Created on: Apr 11, 2011
 *      Author: fumagalli
 */

#include "../include/BC.h"

BC::BC ( getfem::mesh& mesh,
         const std::string& MeshType,
         const ElementDimension& dimension ) :
    M_meshFEM(mesh)
{

    // Dual variable spaces
    //getfem::pfem FETypePressure = getfem::fem_descriptor(FEMType);

    //M_meshFEM.set_finite_element(mesh.convex_index(), FETypePressure);
    bgeot::pgeometric_trans geometricTransformation;

    geometricTransformation = bgeot::geometric_trans_descriptor(MeshType);

    M_meshFEM.set_finite_element(mesh.convex_index(), getfem::classical_fem(
            geometricTransformation, 1));

    // Flag for fracture boundary conditions
    // Dirichlet boundary condition
    M_dirichlet.clear();
    M_dirichlet.push_back(DIRICHLET_BOUNDARY_NUM);

    // Neumann boundary condition
    M_neumann.clear();
    M_neumann.push_back(NEUMANN_BOUNDARY_NUM);

    // All boundary conditions
    M_extBoundary.clear();
    M_extBoundary.push_back(DIRICHLET_BOUNDARY_NUM);
    M_extBoundary.push_back(NEUMANN_BOUNDARY_NUM);

    sizeVector_Type boundaryFlags(2);
    boundaryFlags [ 0 ] = DIRICHLET_BOUNDARY_NUM;
    boundaryFlags [ 1 ] = NEUMANN_BOUNDARY_NUM;

    sizeVectorContainer_Type boundary_cv;
    sizeVectorContainer_Type boundary_flags;

    // Number of Neumann boundary condition
    size_type neumannBoundary = 0;

    // List all the convexes
    dal::bit_vector nn = mesh.convex_index();
    bgeot::size_type i_cv;

    for ( i_cv << nn; i_cv != bgeot::size_type(-1); i_cv << nn )
    {
        neumannBoundary++;
    }

    boundary_cv.resize(neumannBoundary);
    boundary_flags.resize(neumannBoundary);

    getfem::mesh_region borderFaces;
    getfem::outer_faces_of_mesh(mesh, borderFaces);

    if ( dimension == MEDIUM )
    {
        for ( getfem::mr_visitor i(borderFaces); !i.finished(); ++i )
        {
            assert(i.is_face());

            base_node un = mesh.normal_of_face_of_convex(i.cv(), i.f());
            un /= gmm::vect_norm2(un);

            if  (false)  //( gmm::abs(gmm::abs(un [ dimension - 1 ]) - 1.0) > 1.0E-7 )
            {
                // Dirichlet, flag 0
                boundary_cv [ i.cv() ].push_back(i.f());
                boundary_flags [ i.cv() ].push_back(boundaryFlags [ 0 ]);
                //This        will enforce M_mediumMesh.region(0).add(i.cv(), i.f());
            }
            else
            {
                // Neumann, flag 1
                boundary_cv [ i.cv() ].push_back(i.f());
                boundary_flags [ i.cv() ].push_back(boundaryFlags [ 1 ]);
                // This will enforce M_mediumMesh.region(1).add(i.cv(), i.f());
            }
        }
    }
    else
    {

        for ( getfem::mr_visitor i(borderFaces); !i.finished(); ++i )
        {
            assert(i.is_face());
            base_node un = mesh.normal_of_face_of_convex(i.cv(), i.f());
            un /= gmm::vect_norm2(un);

            if (false)//( gmm::abs(gmm::abs(un [ dimension - 1 ]) - 1.0) > 1.0E-7 )
            {
            // Dirichlet, flag 0
            boundary_cv [ i.cv() ].push_back(i.f());
            boundary_flags [ i.cv() ].push_back(boundaryFlags [ 1 ]);
            //This        will enforce M_mediumMesh.region(0).add(i.cv(), i.f());
}
 else
            {
                // Neumann, flag 1
                boundary_cv [ i.cv() ].push_back(i.f());
                boundary_flags [ i.cv() ].push_back(boundaryFlags [1 ]);
                // This will enforce M_mediumMesh.region(1).add(i.cv(), i.f());
            }
        }
    }
    // Bulk mesh
    for ( size_type cv = 0; cv < boundary_flags.size(); ++cv )
    {
        for ( size_type j = 0; j < boundary_flags [ cv ].size(); j++ )
        {
            mesh.region(boundary_flags [ cv ] [ j ]).add(cv,
                    boundary_cv [ cv ] [ j ]);
        }
    }

}
