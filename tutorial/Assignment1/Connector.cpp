//
// Created by ditto on 26/11/2022.
//

#include <per_vertex_normals.h>
#include "Connector.h"
#include "Model.h"


Connector::Connector(std::shared_ptr<cg3d::Mesh> mesh): originalMesh(mesh) {
    OV = mesh->data[0].vertices;
    OF = mesh->data[0].faces;
}

std::shared_ptr<cg3d::Mesh> Connector::reset(igl::opengl::glfw::Viewer *viewer) {
    V = OV;
    F = OF;
    igl::edge_flaps(F,E,EMAP,EF,EI);
    C.resize(E.rows(),V.cols());
    Eigen::VectorXd costs(E.rows());
    // https://stackoverflow.com/questions/2852140/priority-queue-clear-method
    // queue.clear();
    queue = {};
    EQ = Eigen::VectorXi::Zero(E.rows());
    {
        Eigen::VectorXd costs(E.rows());
        igl::parallel_for(E.rows(),[&](const int e)
        {
            double cost = e;
            Eigen::RowVectorXd p(1,3);
            igl::shortest_edge_and_midpoint(e,V,F,E,EMAP,EF,EI,cost,p);
            C.row(e) = p;
            costs(e) = cost;
        },10000);
        for(int e = 0;e<E.rows();e++)
        {
            queue.emplace(costs(e), e, 0);
        }
    }

    num_collapsed = 0;
    Eigen::MatrixXd vertexNormals;
    igl::per_vertex_normals(V,F,vertexNormals);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Zero(V.rows(),2);
    std::vector<cg3d::MeshData> newMeshDataList;
    newMeshDataList.push_back({V, F, vertexNormals, textureCoords});
    std::shared_ptr<cg3d::Mesh> newMesh = std::make_shared<cg3d::Mesh>("modified mesh", newMeshDataList);
    return newMesh;
}


std::shared_ptr<cg3d::Mesh> Connector::simplifyMesh(igl::opengl::glfw::Viewer *viewer, int numberOfFacesToDelete) {
    // If animating then collapse 10% of edges
    if(!queue.empty())
    {
        bool something_collapsed = false;
        // collapse edge
        const int max_iter = std::ceil(numberOfFacesToDelete);
        for(int j = 0;j<max_iter;j++)
        {
            if(!igl::collapse_edge(igl::shortest_edge_and_midpoint, V, F, E, EMAP, EF, EI, queue, EQ, C))
            {
                break;
            }
            something_collapsed = true;
            num_collapsed++;
        }

        if(something_collapsed)
        {
            Eigen::MatrixXd vertexNormals;
            igl::per_vertex_normals(V,F,vertexNormals);
            Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Zero(V.rows(),2);
            std::vector<cg3d::MeshData> newMeshDataList;
            newMeshDataList.push_back({V, F, vertexNormals, textureCoords});
            std::shared_ptr<cg3d::Mesh> newMesh = std::make_shared<cg3d::Mesh>("modified mesh", newMeshDataList);
            return newMesh;
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

std::shared_ptr<cg3d::Mesh> Connector::simplify(igl::opengl::glfw::Viewer *viewer, int numberOfFacesToDelete) {
    std::vector<std::shared_ptr<cg3d::Mesh>> newMeshList;
   return simplifyMesh(viewer, numberOfFacesToDelete);

}

std::shared_ptr<cg3d::Mesh> Connector::simplifyTenPercent(igl::opengl::glfw::Viewer *viewer) {
    int facesToDelete =  std::ceil(queue.size() * 0.01);
    return simplify(viewer,facesToDelete);
}

