#pragma once

#include <iostream>
#include "Mesh.h"
#include "Material.h"
#include "Movable.h"
#include "ViewerData.h"


namespace cg3d
{

class Model : virtual public Movable
{
    friend class DrawVisitor;

public:
    ~Model() override = default;

    static std::shared_ptr<Model> Create(const std::string& file, std::shared_ptr<Material> material, const std::shared_ptr<Movable>& parent = nullptr);
    static std::shared_ptr<Model> Create(std::string name, const std::string& file, std::shared_ptr<Material> material, const std::shared_ptr<Movable>& parent = nullptr);
    static std::shared_ptr<Model> Create(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const std::shared_ptr<Movable>& parent = nullptr);
    static std::shared_ptr<Model> Create(std::string name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const std::shared_ptr<Movable>& parent = nullptr);
    static std::shared_ptr<Model> Create(std::string name, std::vector<std::shared_ptr<Mesh>> meshList, std::shared_ptr<Material> material, const std::shared_ptr<Movable>& parent = nullptr);

    void Accept(Visitor* visitor) override;

    std::shared_ptr<const Material> material;
    bool showFaces = true;
    bool showTextures = true;
    bool showWireframe = false;
    bool isHidden = false;
    Eigen::Vector4f wireframeColor{0, 0, 0, 0};
    int meshIndex = 0;

    inline const std::vector<std::shared_ptr<Mesh>>& GetMeshList() const { return meshList; }
    void SetMeshList(std::vector<std::shared_ptr<Mesh>> _meshList);

    // helper functions
    static void UpdateDataAndBindMesh(igl::opengl::ViewerData& viewerData, const Program& program);
    void UpdateDataAndDrawMeshes(const Program& program, bool _showFaces, bool bindTextures);

protected:
    // protected constructor (use factory method to create models)
    Model(std::string name, std::vector<std::shared_ptr<Mesh>> meshList, std::shared_ptr<Material> material = nullptr);

private:
    static std::vector<igl::opengl::ViewerData> CreateViewerData(const std::shared_ptr<Mesh>& mesh);
    std::vector<std::shared_ptr<Mesh>> meshList;
    std::vector<std::vector<igl::opengl::ViewerData>> viewerDataListPerMesh;

    // TODO: TAL: handle the colors...
    Eigen::RowVector4f ambient = Eigen::RowVector4f(1.0, 1.0, 1.0, 1.0);
    Eigen::RowVector4f diffuse = Eigen::RowVector4f(1.0, 1.0, 1.0, 1.0);
    Eigen::RowVector4f specular = Eigen::RowVector4f(1.0, 1.0, 1.0, 1.0);
};

} // namespace cg3d
