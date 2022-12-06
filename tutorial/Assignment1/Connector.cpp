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
        calculateCallBack();
        Eigen::VectorXd costs(E.rows());

        igl::parallel_for(E.rows(),[&](const int e)
        {
            double cost = e;
            Eigen::RowVectorXd p(1,3);
            costAndPlacementCallback(e,V,F,E,EMAP,EF,EI,cost,p);
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
    int collapsedBefore = num_collapsed;

    if(!queue.empty())
    {
        bool something_collapsed = false;
        // collapse edge
        const int max_iter = std::ceil(numberOfFacesToDelete);
        for(int j = 0;j<max_iter;j++)
        {
            auto next = queue.top();
            auto cost = std::get<0>(next);
            auto v1Index = std::get<1>(next);
            auto v2Index = std::get<2>(next);
            auto v1 = V.row(v2Index);
            auto v2 = V.row(v2Index);
            if(!igl::collapse_edge(costAndPlacementCallback, V, F, E, EMAP, EF, EI, queue, EQ, C))
            {
                break;
            }
            std::cout << "Collapsed edge in: (" << v1 << "), (" << v2 << ") with cost: " << cost << std::endl;
            std::cout << "New position is: (" << VTags[v1Index](0, 0) << " " << VTags[v1Index](1, 0) << " " <<  VTags[v1Index](2, 0) << "), (" <<
                VTags[v2Index](0, 0) << " " << VTags[v2Index](1, 0) << " " << VTags[v2Index](2, 0)  << ")" << std::endl;

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
            std::cout << "Collapsed " << num_collapsed - collapsedBefore << std::endl;
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

Eigen::MatrixXd to4D(Eigen::MatrixXd vec, bool isNormal= false) {
    bool shouldTranspose = true;
    Eigen::MatrixXd vecCopy;
    if(vec.rows() == 1 && vec.cols() == 3) {
        vecCopy = vec.transpose();
        shouldTranspose = false;
    } else {
        vecCopy = vec;
    }

    Eigen::Vector4d res;
    res.x() = vecCopy(0, 0);
    res.y() = vecCopy(1, 0);
    res.z() = vecCopy(2,0);
    if(isNormal) {
        res.w() = 0;
    } else {
        res.w() = 1;
    }
    if(shouldTranspose)
        res = res.transpose();

    return res;

}

Eigen::MatrixXd diag4D(double diagVal) {
    Eigen::Matrix4d diag;
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            if(i==j) {
                diag(i,j) = diagVal;
            } else {
                diag(i,j) = 0;
            }
        }
    }
    return diag.transpose();

}

void Connector::calculateCallBack() {
    Eigen::MatrixXd vertexNormals;
    igl::per_vertex_normals(V, F, vertexNormals);
    for(int i = 0; i<V.rows(); i++) {
        Eigen::MatrixXd normal = to4D(vertexNormals.row(i));
        Eigen::MatrixXd transposedNormal = normal.transpose();
        Eigen::MatrixXd vertex = to4D(V.row(i));
        Eigen::MatrixXd transposedVertex = vertex.transpose();
        Eigen::MatrixXd d = -(transposedNormal * vertex);
        double dVal = d(0,0);
        Eigen::MatrixXd m = (2.0*dVal*transposedNormal);
        Eigen::MatrixXd m2 = m*vertex;
        Eigen::MatrixXd dSquare = d * d;

        Eigen::MatrixXd q1 = transposedVertex * (normal * transposedNormal) * vertex;
        Eigen::MatrixXd q2 =  2*(dVal*vertex).transpose()*vertex;

        Eigen::MatrixXd q = diag4D( q1(0,0))  + diag4D(q2(0,0)) + diag4D(dVal*dVal);

        Qs[i] = q;
    }

    for(int j = 0; j<E.rows(); j++) {
        int i1 = E(j,0);
        int i2 = E(j,1);

        Eigen::MatrixXd qTag = Qs[i1] + Qs[i2];
        QTags[j] = qTag;

        Eigen::Matrix4d matrixToInv;
        matrixToInv.row(0) = qTag.row(0);
        matrixToInv.row(1) = qTag.row(1);
        matrixToInv.row(2) = qTag.row(2);
        matrixToInv(3, 0) = 0;
        matrixToInv(3, 1) = 0;
        matrixToInv(3, 2) = 0;
        matrixToInv(3, 3) = 1;

        Eigen::FullPivLU<Eigen::MatrixXd> lu(matrixToInv);
        Eigen::Vector4d res;
        if(lu.isInvertible()) {
            Eigen::MatrixXd inv = lu.inverse();
            Eigen::MatrixXd ones = Eigen::Vector4d(0, 0, 0, 1);
            Eigen::MatrixXd res2 = inv * ones;
            res = res2;
        } else {
            Eigen::MatrixXd sum = 0.5 * static_cast<Eigen::MatrixXd>(to4D(V.row(i1)) + to4D(V.row(i2)));
            res = sum;
        }

        VTags[j] = res;
    }



    costAndPlacementCallback =  [this] (const int e, const Eigen::MatrixXd & V, const Eigen::MatrixXi & F, const Eigen::MatrixXi & E, const Eigen::VectorXi &EMAP,
                                        const Eigen::MatrixXi & EF, const Eigen::MatrixXi &EI, double & cost, Eigen::RowVectorXd & p) {
        Eigen::MatrixXd qTag = QTags.at(e);
        Eigen::MatrixXd vTag = VTags.at(e);
        p = Eigen::Vector3d(vTag(0,0), vTag(1,0), vTag(2,0));
        Eigen::MatrixXd transposed = vTag.transpose();
        Eigen::MatrixXd costMat = transposed * qTag * vTag;
        cost = costMat(0,0);
    };



}


