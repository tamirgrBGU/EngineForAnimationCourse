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
    Eigen::MatrixXd V, OV;
    Eigen::MatrixXi F, OF;
    Eigen::VectorXi EQ;
    Eigen::MatrixXi E;
    Eigen::VectorXi EMAP;
    Eigen::MatrixXi EF;
    Eigen::MatrixXi EI;
    igl::min_heap< std::tuple<double,int,int> > queue;
    Eigen::MatrixXd C;
    int num_collapsed;
    std::shared_ptr<cg3d::Mesh>  simplify(igl::opengl::glfw::Viewer *viewer, int numberOfFacesToDelete);
    std::shared_ptr<cg3d::Mesh> simplifyMesh(igl::opengl::glfw::Viewer *viewer, int numberOfFacesToDelete);
    std::map<std::pair<int, int>, Eigen::Matrix<double, 4, 4>> Qs;

public:
    Connector(std::shared_ptr<cg3d::Mesh> mesh);
    std::shared_ptr<cg3d::Mesh> originalMesh;
    std::shared_ptr<cg3d::Mesh> simplifyTenPercent(igl::opengl::glfw::Viewer *viewer);
    std::shared_ptr<cg3d::Mesh> reset(igl::opengl::glfw::Viewer *viewer);
};


#endif //ENGINEREWORK_CONNECTOR_H
