#include "ObjLoader.h"

#pragma warning(push, 3)
#pragma warning (disable: 4244) // disable annoying warnings from OBJ_Loader.h
// note: do not move this include file to ObjLoader.h (header-only library - will cause linker errors)
#include "OBJ_Loader/OBJ_Loader.h"
#pragma warning(pop)

#include "Mesh.h"
#include "Model.h"
#include "Utility.h"
#include <memory>
#include <algorithm>


namespace cg3d
{

MeshData ObjLoader::MeshDataFromObjLoader(const objl::Mesh& loadedMesh)
{
    const auto& vertices = loadedMesh.Vertices;
    const auto& indices = loadedMesh.Indices;

    Eigen::MatrixXd V(vertices.size(), 3);
    Eigen::MatrixXi F(indices.size() / 3, 3);
    Eigen::MatrixXd V_normals(vertices.size(), 3);
    Eigen::MatrixXd V_uv(vertices.size(), 2);

    // import the vertices data
    int j = 0;
    for (const auto& vertex: vertices) {
        V(j, 0) = vertex.Position.X;
        V(j, 1) = vertex.Position.Y;
        V(j, 2) = vertex.Position.Z;
        V_normals(j, 0) = vertex.Normal.X;
        V_normals(j, 1) = vertex.Normal.Y;
        V_normals(j, 2) = vertex.Normal.Z;
        V_uv(j, 0) = vertex.TextureCoordinate.X;
        V_uv(j, 1) = vertex.TextureCoordinate.Y;
        j++;
    }

    // import the faces data
    F.resize(int(indices.size()) / 3, 3);
    j = 0;
    for (auto index: indices) {
        F(j / 3, j % 3) = int(index);
        j++;
    }

    // todo: add face normals?

    return {V, F, V_normals, V_uv};
}

std::vector<MeshData> ObjLoader::MeshDataListFromObjLoader(const objl::Loader& loader)
{
    std::vector<MeshData> dataList;
    for (auto& mesh: loader.LoadedMeshes)
        dataList.emplace_back(MeshDataFromObjLoader(mesh));

    return std::move(dataList);
}

std::shared_ptr<Mesh> ObjLoader::MeshFromObj(std::string name, const std::vector<std::string>& files)
{
    objl::Loader loader;
    std::vector<MeshData> dataList;

    for (const auto& file: files) {
        loader.LoadFile(file);
        auto moreData{MeshDataListFromObjLoader(loader)};
        std::move(moreData.begin(), moreData.end(), std::back_inserter(dataList));
    }

    return std::make_shared<Mesh>(std::move(name), dataList);
}

std::shared_ptr<Mesh> ObjLoader::MeshFromObj(std::string name, std::istream& in)
{
    objl::Loader loader;
    loader.LoadFile(in);
    return MeshFromObjLoader(std::move(name), loader);
}

std::shared_ptr<Mesh> ObjLoader::MeshFromObjLoader(std::string name, const objl::Mesh& loadedMesh)
{
    return std::make_shared<Mesh>(std::move(name), std::vector<MeshData>{MeshDataFromObjLoader(loadedMesh)});
}

std::shared_ptr<Mesh> ObjLoader::MeshFromObjLoader(std::string name, objl::Loader& loader)
{
    return std::make_shared<Mesh>(std::move(name), MeshDataListFromObjLoader(loader));
}

std::shared_ptr<Model> ObjLoader::ModelFromObj(std::string name, std::istream& in, std::shared_ptr<Material> material)
{
    objl::Loader loader;
    loader.LoadFile(in);
    return ModelFromObjLoader(std::move(name), loader, std::move(material));
}

std::shared_ptr<Model> ObjLoader::ModelFromObj(std::string name, const std::string& file, std::shared_ptr<Material> material)
{
    objl::Loader loader;
    loader.LoadFile(file);
    return std::move(ModelFromObjLoader(std::move(name), loader, std::move(material)));
}

std::shared_ptr<Model> ObjLoader::ModelFromObjLoader(std::string name, objl::Loader& loader, std::shared_ptr<Material> material)
{
    std::vector<std::shared_ptr<Mesh>> meshList;

    auto& meshes = loader.LoadedMeshes;
    for (int i = 0; i < meshes.size(); i++)
        meshList.emplace_back(MeshFromObjLoader(name + "_mesh" + (meshes.size() > 1 ? std::to_string(i + 1) : ""), meshes[i]));

    return Model::Create(std::move(name), meshList, std::move(material));
}

} // namespace cg3d
