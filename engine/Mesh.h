#pragma once

#include <Eigen/Core>
#include <iostream>
#include <utility>
#include <vector>


namespace cg3d
{

struct MeshData
{
    const Eigen::MatrixXd vertices; // Vertices of the mesh (#V x 3)
    const Eigen::MatrixXi faces; // Faces of the mesh (#F x 3)
    const Eigen::MatrixXd vertexNormals; // One normal per vertex
    const Eigen::MatrixXd textureCoords; // UV vertices
};

class Mesh
{
public:
    std::string name;

    std::vector<MeshData> data;

    Mesh(std::string name, Eigen::MatrixXd vertices, Eigen::MatrixXi faces, Eigen::MatrixXd vertexNormals, Eigen::MatrixXd textureCoords);
    Mesh(std::string name, std::vector<MeshData> data) : name(std::move(name)), data(std::move(data)) {};
    Mesh(const Mesh& mesh) = default;

    static const std::shared_ptr<Mesh>& Plane();
    static const std::shared_ptr<Mesh>& Cube();
    static const std::shared_ptr<Mesh>& Tetrahedron();
    static const std::shared_ptr<Mesh>& Octahedron();
    static const std::shared_ptr<Mesh>& Cylinder();

    [[nodiscard]] const Eigen::MatrixXd& GetVertices(int index = 0) const { return data[index].vertices; }
    [[nodiscard]] const Eigen::MatrixXi& GetFaces(int index = 0) const { return data[index].faces; }
    [[nodiscard]] const Eigen::MatrixXd& GetVertexNormals(int index = 0) const { return data[index].vertexNormals; }
    [[nodiscard]] const Eigen::MatrixXd& GetTextureCoords(int index = 0) const { return data[index].textureCoords; }
};

} // namespace cg3d
