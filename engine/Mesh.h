#pragma once

#include <iostream>
#include <Eigen/Core>

using namespace std;
using namespace Eigen;

namespace objl { // can't include OBJ_Loader here because it's a header-only library
    class Loader;
}

class Mesh
{
public:
    const string name;

    explicit Mesh(const string &file);

    static shared_ptr<const Mesh> Plane();
    static shared_ptr<const Mesh> Cube();
    static shared_ptr<const Mesh> Tetrahedron();
    static shared_ptr<const Mesh> Octahedron();
    static shared_ptr<const Mesh> Cylinder();

    // TODO: TAL: make these private
    const MatrixXd V; // Vertices of the current mesh (#V x 3)
    const MatrixXi F; // Faces of the mesh (#F x 3)
    const MatrixXd V_normals; // One normal per vertex
    const MatrixXd V_uv; // UV vertices

private:

    Mesh(const string &name, MatrixXd V, MatrixXi F, MatrixXd V_normals, MatrixXd V_uv);
    static Mesh LoadFile(const string &file);
    static Mesh LoadFile(const string& name, istream& in);
    static Mesh LoadFile(const string& name, objl::Loader& loader);
};
