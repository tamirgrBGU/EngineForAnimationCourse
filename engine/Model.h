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

protected:
    Model(const std::string& file, std::shared_ptr<Material> material);
    Model(std::string name, const std::string& file, std::shared_ptr<Material> material);
    Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    Model(std::string name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    Model(std::string name, std::vector<std::shared_ptr<Mesh>> meshList, std::shared_ptr<Material> material);
    Model(const Model& other) = default; // important: doesn't add itself to the parent's children (object isn't constructed yet)
    Model(Model&&) = default; // important: doesn't add itself to the parent's children (object isn't constructed yet)
    Model& operator=(const Model& other) = default;

public:
    template<typename... Args>
    static std::shared_ptr<Model> Create(Args&&... args) {
        return std::shared_ptr<Model>{new Model{std::forward<Args>(args)...}}; // NOLINT(modernize-make-shared)
    }

    ~Model() override = default;

    void Accept(Visitor* visitor) override { visitor->Visit(this); };

    std::shared_ptr<Material> material;
    bool isHidden = false;
    bool showFaces = true;
    bool showTextures = true;
    bool showWireframe = false;
    Eigen::Vector4f wireframeColor{0, 0, 0, 0};
    int meshIndex = 0;
    int mode = 0;

    inline std::shared_ptr<Mesh> GetMesh(int index = 0) const { return meshList[index]; }
    inline std::vector<std::shared_ptr<Mesh>> GetMeshList() const { return meshList; }
    void SetMeshList(std::vector<std::shared_ptr<Mesh>> _meshList);
    void UpdateDataAndDrawMeshes(const Program& program, bool _showFaces, bool bindTextures); // helper function
    void AddOverlay(const OverlayData& data, bool drawPoints);
private:
    static void UpdateDataAndBindMesh(igl::opengl::ViewerData& viewerData, const Program& program); // helper function

    static std::vector<igl::opengl::ViewerData> CreateViewerData(const std::shared_ptr<Mesh>& mesh);
    std::vector<std::shared_ptr<Mesh>> meshList;
    std::vector<std::vector<igl::opengl::ViewerData>> viewerDataListPerMesh;

    // TODO: TAL: handle the colors...
    Eigen::RowVector4f ambient = Eigen::RowVector4f(1.0, 1.0, 1.0, 1.0);
    Eigen::RowVector4f diffuse = Eigen::RowVector4f(1.0, 1.0, 1.0, 1.0);
    Eigen::RowVector4f specular = Eigen::RowVector4f(1.0, 1.0, 1.0, 1.0);
};

} // namespace cg3d
