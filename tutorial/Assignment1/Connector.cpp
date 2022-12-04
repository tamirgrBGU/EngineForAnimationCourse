//
// Created by ditto on 26/11/2022.
//

#include <per_vertex_normals.h>

#include <utility>
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
    calculateCallBack();


    if(!queue.empty())
    {
        bool something_collapsed = false;
        // collapse edge
        const int max_iter = std::ceil(numberOfFacesToDelete);
        for(int j = 0;j<max_iter;j++)
        {
            if(!igl::collapse_edge(costAndPlacementCallback, V, F, E, EMAP, EF, EI, queue, EQ, C))
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

void Connector::calculateCallBack() {
    Eigen::MatrixXd vertexNormals;
    igl::per_vertex_normals(V, F, vertexNormals);
    std::map<int, Eigen::MatrixXd> Qs;
    std::map<int, Eigen::MatrixXd> VTags;
    std::map<int, Eigen::MatrixXd> QTags;



    for(int i = 0; i<V.rows(); i++) {
        Eigen::MatrixXd normal = vertexNormals.row(i);
        Eigen::MatrixXd transposedNormal = normal.transpose();
        Eigen::MatrixXd vertex = V.row(i);
        Eigen::MatrixXd transposedVertex = vertex.transpose();
        Eigen::MatrixXd d = -(transposedNormal * vertex);
        double dVal = d.row(0)[0];
        Eigen::MatrixXd m = (2.0*dVal*transposedNormal);
        Eigen::MatrixXd m2 = m*vertex;
        Eigen::MatrixXd dSquare = d * d;

        Eigen::MatrixXd q1 = transposedVertex * (normal * transposedNormal) * vertex;
        Eigen::MatrixXd q2 = static_cast<Eigen::MatrixXd>(2 * (dVal*normal).transpose())*vertex;

        double dValSquare = dVal*dVal;
        Eigen::DiagonalMatrix<double, 3> diag(dValSquare, dValSquare, dValSquare);
        Eigen::MatrixXd q = static_cast<Eigen::MatrixXd>(q1 + q2) + static_cast<Eigen::MatrixXd>(diag);



        Eigen::Matrix4d newQ;
        for(int k=0; k<3; k++) {
            for(int l=0; l<3; l++) {
                newQ(k, l) = q(k, l);
            }
        }


        newQ(3,0) = 0;
        newQ(3,1) = 0;
        newQ(3,2) = 0;

        newQ(0, 3) = 0;
        newQ(1, 3) = 0;
        newQ(2, 3) = 0;

        newQ(3,3) = 1;

        Qs[i] = newQ;
    }

    for(int j = 0; j<E.rows(); j++) {
        int i1 = E.row(j)[0];
        int i2 = E.row(j)[1];

        Eigen::MatrixXd qTag = Qs[i1] + Qs[i2];
        QTags[j] = qTag;
        Eigen::FullPivLU<Eigen::MatrixXd> lu(qTag);
        Eigen::Vector4d res;
        if(lu.isInvertible()) {
            Eigen::MatrixXd inv = lu.inverse();
            Eigen::MatrixXd ones = Eigen::Vector4d(0, 0, 0, 1);
            res = inv * ones;
        } else {
            Eigen::MatrixXd sum = (0.5 * static_cast<Eigen::MatrixXd>(V.row(i1) + V.row(i2)));
            res.x() = sum(0, 0);
            res.y() = sum(0, 1);
            res.z() = sum(0, 2);
            res.w() = 1;
        }

        VTags[j] = res;
    }



    costAndPlacementCallback =  [QTags, VTags] (const int e, const Eigen::MatrixXd & V, const Eigen::MatrixXi & F, const Eigen::MatrixXi & E, const Eigen::VectorXi &EMAP,
                                        const Eigen::MatrixXi & EF, const Eigen::MatrixXi &EI, double & cost, Eigen::RowVectorXd & p) {
        Eigen::MatrixXd qTag = QTags.at(e);
        Eigen::MatrixXd vTag = VTags.at(e);
        p = Eigen::Vector3d(vTag(0,0), vTag(1,0), vTag(2,0));
        Eigen::MatrixXd transposed = vTag.transpose();
        Eigen::MatrixXd costMat = transposed * qTag * vTag;
        cost = costMat(0,0);
    };



}

