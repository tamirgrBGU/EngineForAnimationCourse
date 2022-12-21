#include "Model.h"
#include "Visitor.h"
#include "ViewerData.h"
#include "Movable.h"
#include "ObjLoader.h"
#include <filesystem>
#include <utility>


namespace cg3d
{

namespace fs = std::filesystem;

Model::Model(const std::string& file, std::shared_ptr<Material> material)
        : Model{fs::path(file).filename().stem().generic_string(), file, std::move(material)} {}

Model::Model(std::string name, const std::string& file, std::shared_ptr<Material> material)
        : Model{*ObjLoader::ModelFromObj(std::move(name), file, std::move(material))} {}

Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
        : Model{mesh->name + "_model", std::move(mesh), std::move(material)} {};

Model::Model(std::string name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
        : Model{std::move(name), std::vector<std::shared_ptr<Mesh>>{{std::move(mesh)}}, std::move(material)} {}

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
      //  viewerData.compute_normals(); // todo: implement (this overwrites both face and vertex normals even if either is already present)
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

// void Model::AddOverlay(Eigen::MatrixXd points,Eigen::MatrixXd edges, Eigen::MatrixXd colors)
void Model::AddOverlay(const OverlayData& data, bool drawPoints)
{
    viewerDataListPerMesh[0][0].show_overlay = 3;
 //   viewerDataListPerMesh[0][0].show_faces = -1;
    if(drawPoints)
        viewerDataListPerMesh[0][0].add_points(data.points, data.colors);
    else
        viewerDataListPerMesh[0][0].add_edges(data.points, data.edges, data.colors); 
}

void Model::UpdateDataAndDrawMeshes(const Program& program, bool _showFaces, bool bindTextures)
{
    for (auto& viewerDataList: viewerDataListPerMesh) {
        auto& viewerData = viewerDataList[std::min(meshIndex, int(viewerDataList.size() - 1))];
        UpdateDataAndBindMesh(viewerData, program);
        if (bindTextures) material->BindTextures();
        if (mode == 0)
            viewerData.meshgl.draw_mesh(_showFaces);
        else
            viewerData.meshgl.draw_lines();
        // viewerData.meshgl.bind_overlay_points();
        // viewerData.meshgl.draw_overlay_points();
        // viewerData.meshgl.bind_overlay_lines();
    }
}

void Model::SetMeshList(std::vector<std::shared_ptr<Mesh>> _meshList)
{
    meshList = std::move(_meshList);
    viewerDataListPerMesh.clear();
    for (auto& mesh: meshList)
        viewerDataListPerMesh.emplace_back(CreateViewerData(mesh));
}

} // namespace cg3d
