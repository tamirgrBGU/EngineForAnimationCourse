#include "Model.h"
#include "Visitor.h"
#include "ViewerData.h"
#include "Movable.h"
#include "ObjLoader.h"
#include <filesystem>
#include <utility>

namespace fs = std::filesystem;


namespace cg3d
{

std::shared_ptr<Model> Model::Create(const std::string& file, std::shared_ptr<Material> material, const std::shared_ptr<Movable>& parent)
{
    std::string name = fs::path(file).filename().stem().generic_string();
    return Create(name, file, std::move(material), parent);
}

std::shared_ptr<Model>
Model::Create(std::string name, const std::string& file, std::shared_ptr<Material> material, const std::shared_ptr<Movable>& parent)
{
    auto model{ObjLoader::ModelFromObj(std::move(name), file, std::move(material))};
    model->SetParent(parent);
    return std::move(model);
}

std::shared_ptr<Model> Model::Create(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const std::shared_ptr<Movable>& parent)
{
    std::string name = mesh->name + "_model"; // construct the name separately to avoid warnings
    return Create(std::move(name), std::move(mesh), std::move(material), parent);
}

std::shared_ptr<Model> Model::Create(std::string name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const std::shared_ptr<Movable>& parent)
{
    return Create(std::move(name), std::vector<std::shared_ptr<Mesh>>{{std::move(mesh)}}, std::move(material), parent);
}

std::shared_ptr<Model>
Model::Create(std::string name, std::vector<std::shared_ptr<Mesh>> meshList, std::shared_ptr<Material> material, const std::shared_ptr<Movable>& parent)
{
    auto model = std::shared_ptr<Model>(new Model(std::move(name), std::move(meshList), std::move(material)));
    model->SetParent(parent);
    return std::move(model);
}

Model::Model(std::string name, std::vector<std::shared_ptr<Mesh>> meshList, std::shared_ptr<Material> material)
        : Movable(std::move(name)), material(std::move(material))
{
    SetMeshList(std::move(meshList));
}

std::vector<igl::opengl::ViewerData> Model::CreateViewerData(const std::shared_ptr<Mesh>& mesh)
{
    std::vector<igl::opengl::ViewerData> dataList;

    for (auto& meshData: mesh->data) {
        igl::opengl::ViewerData viewerData;
        viewerData.set_mesh(meshData.vertices, meshData.faces);
        viewerData.set_uv(meshData.textureCoords);
        viewerData.set_normals(meshData.vertexNormals);
        viewerData.line_width = 1.0f;
        viewerData.uniform_colors(Eigen::Vector3d(1.0, 1.0, 1.0), Eigen::Vector3d(1.0, 1.0, 1.0), Eigen::Vector3d(1.0, 1.0, 1.0)); // todo: implement colors
        viewerData.compute_normals(); // todo: implement (this overwrites both face and vertex normals even if either is already present)
        if (viewerData.V_uv.rows() == 0)
            viewerData.grid_texture();
        viewerData.is_visible = 1;
        viewerData.show_overlay = 0;
        dataList.emplace_back(std::move(viewerData));
    }

    return dataList;
}

void Model::UpdateDataAndBindMesh(igl::opengl::ViewerData& viewerData, const Program& program)
{
    viewerData.dirty = igl::opengl::MeshGL::DIRTY_NONE;
    viewerData.updateGL(viewerData, viewerData.invert_normals, viewerData.meshgl);
    viewerData.meshgl.shader_mesh = program.GetHandle();
    viewerData.meshgl.bind_mesh();
}

void Model::UpdateDataAndDrawMeshes(const Program& program, bool _showFaces, bool bindTextures)
{
    for (auto& viewerDataList: viewerDataListPerMesh) {
        auto& viewerData = viewerDataList[std::min(meshIndex, int(viewerDataList.size() - 1))];
        UpdateDataAndBindMesh(viewerData, program);
        if (bindTextures) material->BindTextures();
        viewerData.meshgl.draw_mesh(_showFaces);
    }
}

void Model::SetMeshList(std::vector<std::shared_ptr<Mesh>> _meshList)
{
    meshList = std::move(_meshList);
    viewerDataListPerMesh.clear();
    for (auto& mesh: meshList)
        viewerDataListPerMesh.emplace_back(CreateViewerData(mesh));
}

void Model::Accept(Visitor* visitor)
{
    Movable::Accept(visitor);

    visitor->Visit(this);
}

} // namespace cg3d
