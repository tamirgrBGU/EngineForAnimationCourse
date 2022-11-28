//
// Created by ditto on 26/11/2022.
//
#include <Eigen/Core>
#include <igl/min_heap.h>
#include <igl/parallel_for.h>
#include <igl/read_triangle_mesh.h>
#include <igl/edge_flaps.h>
#include <igl/shortest_edge_and_midpoint.h>
#include <igl/collapse_edge.h>

#include "glfw/Viewer.h"
#include "Mesh.h"
#include "Model.h"

#ifndef ENGINEREWORK_CONNECTOR_H
#define ENGINEREWORK_CONNECTOR_H

class ConnectorException : public std::exception {};


class Connector {
private:
    Eigen::MatrixXi E;
    Eigen::VectorXi EMAP;
    Eigen::MatrixXi EF;
    Eigen::MatrixXi EI;
    igl::min_heap< std::tuple<double,int,int> > Q;
    Eigen::MatrixXd C;
    int num_collapsed;
    bool simplify(igl::opengl::glfw::Viewer *viewer, int numberOfFacesToDelete);
    bool simplifyMesh(igl::opengl::glfw::Viewer *viewer, int numberOfFacesToDelete, std::shared_ptr<cg3d::Mesh> mesh);

public:
    Connector(std::shared_ptr<cg3d::Model> model);
    std::shared_ptr<cg3d::Model> originalModel;
    bool simplifyTenPercent(igl::opengl::glfw::Viewer *viewer);

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Eigen::VectorXi EQ;




};


#endif //ENGINEREWORK_CONNECTOR_H
