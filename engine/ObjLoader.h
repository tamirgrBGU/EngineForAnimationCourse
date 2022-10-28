#pragma  once

#include <string>
#include <memory>
#include <utility>
#include <vector>


namespace objl
{
class Loader;

struct Mesh;
}

namespace cg3d
{

struct MeshData;
class Mesh;
class Model;
class Material;

struct ObjLoader
{
    static MeshData MeshDataFromObjLoader(const objl::Mesh& loadedMesh);
    static std::vector<MeshData> MeshDataListFromObjLoader(const objl::Loader& loader);

    static std::shared_ptr<Mesh> MeshFromObj(std::string name, const std::vector<std::string>& files);
    static std::shared_ptr<Mesh> MeshFromObj(std::string name, std::istream& in);
    static std::shared_ptr<Mesh> MeshFromObjLoader(std::string name, objl::Loader& loader);

    static std::shared_ptr<Mesh> MeshFromObjLoader(std::string name, const objl::Mesh& loadedMesh);
    static std::shared_ptr<Model> ModelFromObj(std::string name, const std::string& file, std::shared_ptr<Material> material);
    static std::shared_ptr<Model> ModelFromObj(std::string name, std::istream& in, std::shared_ptr<Material> material);
    static std::shared_ptr<Model> ModelFromObjLoader(std::string name, objl::Loader& loader, std::shared_ptr<Material> material);

    template<typename... T> static std::shared_ptr<Mesh> MeshFromObjFiles(std::string name, const T&... files) {
        std::string strings[]{files...};
        return MeshFromObj(std::move(name), std::vector<std::string>({strings, strings + sizeof(strings) / sizeof(strings[0])}));
    }
};

} // namespace cg3d
