#include "Mesh.h"
#include "OBJ_Loader.h"
#include <utility>

using std::make_shared;

Mesh::Mesh(const string &name, MatrixXd V, MatrixXi F, MatrixXd V_normals, MatrixXd V_uv) :
        name(name), V(std::move(V)), F(std::move(F)), V_normals(std::move(V_normals)), V_uv(std::move(V_uv)) {}

Mesh::Mesh(const string &file) : Mesh(LoadFile(file)) {} // using copy elision


Mesh Mesh::LoadFile(const string& name, istream& in)
{
    objl::Loader loader;
    loader.LoadFile(in);
    return LoadFile(name, loader);
}

Mesh Mesh::LoadFile(const string &file)
{
    objl::Loader loader;
    loader.LoadFile(file);
    return LoadFile(file, loader);
}

Mesh Mesh::LoadFile(const string& name, objl::Loader& loader)
{
    // currently using only a single mesh (the first one loaded)
    const auto &vertices = loader.LoadedMeshes[0].Vertices;
    const auto &indices = loader.LoadedMeshes[0].Indices;

    MatrixXd V(vertices.size(), 3);
    MatrixXi F(indices.size() / 3, 3);
    MatrixXd V_normals(vertices.size(), 3);
    MatrixXd V_uv(vertices.size(), 2);

    // import the vertices data
    int i = 0;
    for (const auto &vertex: vertices) {
        V(i, 0) = vertex.Position.X;
        V(i, 1) = vertex.Position.Y;
        V(i, 2) = vertex.Position.Z;
        V_normals(i, 0) = vertex.Normal.X;
        V_normals(i, 1) = vertex.Normal.Y;
        V_normals(i, 2) = vertex.Normal.Z;
        V_uv(i, 0) = vertex.TextureCoordinate.X;
        V_uv(i, 1) = vertex.TextureCoordinate.Y;
        i++;
    }

    // import the faces data
    F.resize(int(indices.size()) / 3, 3);
    i = 0;
    for (auto index: indices) {
        F(i / 3, i % 3) = int(index);
        i++;
    }

    // TODO: TAL: add face normals?

    return {name, V, F, V_normals, V_uv};
}

shared_ptr<const Mesh> Mesh::Plane()
{
    static auto data = istringstream(R"(
v 1.000000 1.000000 0.000000
v -1.000000 1.000000 0.000000
v 1.000000 -1.000000 0.000000
v -1.000000 -1.000000 0.000000
vt 1.000000 0.000000
vt 0.000000 1.000000
vt 0.000000 0.000000
vt 1.000000 1.000000
vn 1.0000 -0.0000 0.0000
s off
f 2/1/1 3/2/1 1/3/1
f 2/1/1 4/4/1 3/2/1
        )");

    static const auto MESH = make_shared<Mesh>(LoadFile("Plane", data));

    return MESH;
}

shared_ptr<const Mesh> Mesh::Cube()
{
    static auto data = istringstream(R"(
# This file uses centimeters as units for non-parametric coordinates.
v -0.500000 -0.500000 0.500000
v 0.500000 -0.500000 0.500000
v -0.500000 0.500000 0.500000
v 0.500000 0.500000 0.500000
v -0.500000 0.500000 -0.500000
v 0.500000 0.500000 -0.500000
v -0.500000 -0.500000 -0.500000
v 0.500000 -0.500000 -0.500000
vt 0.0 0.0
vt 0.0 1.0
vt 1.0 0.0
vt 1.0 1.0
f 1/1 2/2 3/3
f 3/3 2/2 4/4
f 3/1 4/2 5/3
f 5/3 4/2 6/4
f 5/1 6/2 7/3
f 7/3 6/2 8/4
f 7/1 8/2 1/3
f 1/3 8/2 2/4
f 2/1 8/2 4/3
f 4/3 8/2 6/4
f 7/1 1/2 5/3
f 5/3 1/2 3/4
        )");

    static const auto MESH = make_shared<Mesh>(LoadFile("Cube", data));

    return MESH;
}

shared_ptr<const Mesh> Mesh::Octahedron()
{
    static auto data = istringstream(R"(
v 0.000000 1.000000 0.000000
v 1.000000 0.000000 0.000000
v 0.000000 0.000000 -1.000000
v -1.000000 0.000000 0.000000
v 0.000000 -0.000000 1.000000
v 0.000000 -1.000000 -0.000000
s off
f 1 2 3
f 1 3 4
f 1 4 5
f 1 5 2
f 6 3 2
f 6 4 3
f 6 5 4
f 6 2 5
        )");

    static const auto MESH = make_shared<Mesh>(LoadFile("Octahedron", data));

    return MESH;
}

shared_ptr<const Mesh> Mesh::Tetrahedron()
{
    static auto data = istringstream(R"(
v 1 0 0
v 0 1 0
v 0 0 1
v 0 0 0
f 2 4 3
f 4 2 1
f 3 1 2
f 1 3 4
        )");

    static const auto MESH = make_shared<Mesh>(LoadFile("Tetrahedron", data));

    return MESH;
}

shared_ptr<const Mesh> Mesh::Cylinder()
{
    static auto data = istringstream(R"(
# This file uses centimeters as units for non-parametric coordinates.

v -0.800000 -0.403499 -0.131105
v -0.800000 -0.343237 -0.249376
v -0.800000 -0.249376 -0.343237
v -0.800000 -0.131105 -0.403499
v -0.800000 0.000000 -0.424264
v -0.800000 0.131105 -0.403499
v -0.800000 0.249376 -0.343237
v -0.800000 0.343237 -0.249376
v -0.800000 0.403499 -0.131105
v -0.800000 0.424264 0.000000
v -0.800000 0.403499 0.131105
v -0.800000 0.343237 0.249376
v -0.800000 0.249376 0.343237
v -0.800000 0.131105 0.403499
v -0.800000 0.000000 0.424264
v -0.800000 -0.131105 0.403499
v -0.800000 -0.249376 0.343237
v -0.800000 -0.343237 0.249376
v -0.800000 -0.403499 0.131105
v -0.800000 -0.424264 0.000000
v 0.800000 -0.403499 -0.131105
v 0.800000 -0.343237 -0.249376
v 0.800000 -0.249376 -0.343237
v 0.800000 -0.131105 -0.403499
v 0.800000 -0.000000 -0.424264
v 0.800000 0.131105 -0.403499
v 0.800000 0.249376 -0.343237
v 0.800000 0.343237 -0.249376
v 0.800000 0.403499 -0.131105
v 0.800000 0.424264 0.000000
v 0.800000 0.403499 0.131105
v 0.800000 0.343237 0.249376
v 0.800000 0.249376 0.343237
v 0.800000 0.131105 0.403499
v 0.800000 0.000000 0.424264
v 0.800000 -0.131105 0.403499
v 0.800000 -0.249376 0.343237
v 0.800000 -0.343237 0.249376
v 0.800000 -0.403499 0.131105
v 0.800000 -0.424264 0.000000
v -0.800000 0.000000 0.000000
v 0.800000 -0.000000 0.000000
vt 0.648603 0.107966
vt 0.626409 0.064408
vt 0.591842 0.029841
vt 0.548284 0.007647
vt 0.500000 -0.000000
vt 0.451716 0.007647
vt 0.408159 0.029841
vt 0.373591 0.064409
vt 0.351397 0.107966
vt 0.343750 0.156250
vt 0.351397 0.204534
vt 0.373591 0.248091
vt 0.408159 0.282659
vt 0.451716 0.304853
vt 0.500000 0.312500
vt 0.548284 0.304853
vt 0.591841 0.282659
vt 0.626409 0.248091
vt 0.648603 0.204534
vt 0.656250 0.156250
vt 0.375000 0.312500
vt 0.387500 0.312500
vt 0.400000 0.312500
vt 0.412500 0.312500
vt 0.425000 0.312500
vt 0.437500 0.312500
vt 0.450000 0.312500
vt 0.462500 0.312500
vt 0.475000 0.312500
vt 0.487500 0.312500
vt 0.500000 0.312500
vt 0.512500 0.312500
vt 0.525000 0.312500
vt 0.537500 0.312500
vt 0.550000 0.312500
vt 0.562500 0.312500
vt 0.575000 0.312500
vt 0.587500 0.312500
vt 0.600000 0.312500
vt 0.612500 0.312500
vt 0.625000 0.312500
vt 0.375000 0.688440
vt 0.387500 0.688440
vt 0.400000 0.688440
vt 0.412500 0.688440
vt 0.425000 0.688440
vt 0.437500 0.688440
vt 0.450000 0.688440
vt 0.462500 0.688440
vt 0.475000 0.688440
vt 0.487500 0.688440
vt 0.500000 0.688440
vt 0.512500 0.688440
vt 0.525000 0.688440
vt 0.537500 0.688440
vt 0.550000 0.688440
vt 0.562500 0.688440
vt 0.575000 0.688440
vt 0.587500 0.688440
vt 0.600000 0.688440
vt 0.612500 0.688440
vt 0.625000 0.688440
vt 0.648603 0.795466
vt 0.626409 0.751908
vt 0.591842 0.717341
vt 0.548284 0.695147
vt 0.500000 0.687500
vt 0.451716 0.695147
vt 0.408159 0.717341
vt 0.373591 0.751909
vt 0.351397 0.795466
vt 0.343750 0.843750
vt 0.351397 0.892034
vt 0.373591 0.935591
vt 0.408159 0.970159
vt 0.451716 0.992353
vt 0.500000 1.000000
vt 0.548284 0.992353
vt 0.591841 0.970159
vt 0.626409 0.935591
vt 0.648603 0.892034
vt 0.656250 0.843750
vt 0.500000 0.150000
vt 0.500000 0.837500
f 1/21 2/22 21/42
f 21/42 2/22 22/43
f 2/22 3/23 22/43
f 22/43 3/23 23/44
f 3/23 4/24 23/44
f 23/44 4/24 24/45
f 4/24 5/25 24/45
f 24/45 5/25 25/46
f 5/25 6/26 25/46
f 25/46 6/26 26/47
f 6/26 7/27 26/47
f 26/47 7/27 27/48
f 7/27 8/28 27/48
f 27/48 8/28 28/49
f 8/28 9/29 28/49
f 28/49 9/29 29/50
f 9/29 10/30 29/50
f 29/50 10/30 30/51
f 10/30 11/31 30/51
f 30/51 11/31 31/52
f 11/31 12/32 31/52
f 31/52 12/32 32/53
f 12/32 13/33 32/53
f 32/53 13/33 33/54
f 13/33 14/34 33/54
f 33/54 14/34 34/55
f 14/34 15/35 34/55
f 34/55 15/35 35/56
f 15/35 16/36 35/56
f 35/56 16/36 36/57
f 16/36 17/37 36/57
f 36/57 17/37 37/58
f 17/37 18/38 37/58
f 37/58 18/38 38/59
f 18/38 19/39 38/59
f 38/59 19/39 39/60
f 19/39 20/40 39/60
f 39/60 20/40 40/61
f 20/40 1/41 40/61
f 40/61 1/41 21/62
f 2/2 1/1 41/83
f 3/3 2/2 41/83
f 4/4 3/3 41/83
f 5/5 4/4 41/83
f 6/6 5/5 41/83
f 7/7 6/6 41/83
f 8/8 7/7 41/83
f 9/9 8/8 41/83
f 10/10 9/9 41/83
f 11/11 10/10 41/83
f 12/12 11/11 41/83
f 13/13 12/12 41/83
f 14/14 13/13 41/83
f 15/15 14/14 41/83
f 16/16 15/15 41/83
f 17/17 16/16 41/83
f 18/18 17/17 41/83
f 19/19 18/18 41/83
f 20/20 19/19 41/83
f 1/1 20/20 41/83
f 21/81 22/80 42/84
f 22/80 23/79 42/84
f 23/79 24/78 42/84
f 24/78 25/77 42/84
f 25/77 26/76 42/84
f 26/76 27/75 42/84
f 27/75 28/74 42/84
f 28/74 29/73 42/84
f 29/73 30/72 42/84
f 30/72 31/71 42/84
f 31/71 32/70 42/84
f 32/70 33/69 42/84
f 33/69 34/68 42/84
f 34/68 35/67 42/84
f 35/67 36/66 42/84
f 36/66 37/65 42/84
f 37/65 38/64 42/84
f 38/64 39/63 42/84
f 39/63 40/82 42/84
f 40/82 21/81 42/84
        )");

    static const auto MESH = make_shared<Mesh>(LoadFile("Cylinder", data));

    return MESH;
}
