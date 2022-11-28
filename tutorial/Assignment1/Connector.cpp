//
// Created by ditto on 26/11/2022.
//

#include "Connector.h"

Connector::Connector(const std::string& filename) {
    igl::read_triangle_mesh(filename,V,F);
}

Connector::Connector(std::shared_ptr<cg3d::Mesh> mesh) {
    V = mesh->GetVertices();
    F = mesh->GetFaces();
}


void Connector::init(igl::opengl::glfw::Viewer *viewer) {
    igl::edge_flaps(F,E,EMAP,EF,EI);
    C.resize(E.rows(),V.cols());
    Eigen::VectorXd costs(E.rows());
    // https://stackoverflow.com/questions/2852140/priority-queue-clear-method
    // Q.clear();
    Q = {};
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
            Q.emplace(costs(e),e,0);
        }
    }

    num_collapsed = 0;
    viewer->data().clear();
    viewer->data().set_mesh(V,F);
    viewer->data().set_face_based(true);
}

bool Connector::simplify(igl::opengl::glfw::Viewer *viewer, int numberOfFacesToDelete) {
    // If animating then collapse 10% of edges
    if(!Q.empty())
    {
        bool something_collapsed = false;
        // collapse edge
        const int max_iter = std::ceil(numberOfFacesToDelete);
        for(int j = 0;j<max_iter;j++)
        {
            igl::
            if(!igl::collapse_edge(igl::shortest_edge_and_midpoint,V,F,E,EMAP,EF,EI,Q,EQ,C))
            {
                break;
            }
            something_collapsed = true;
            num_collapsed++;
        }

        if(something_collapsed)
        {
            viewer->data().clear();
            viewer->data().set_mesh(V,F);
            viewer->data().set_face_based(true);
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool Connector::simplifyTenPercent(igl::opengl::glfw::Viewer *viewer) {
    int facesToDelete =  std::ceil(Q.size()*0.01);
    return simplify(viewer,facesToDelete);
}
