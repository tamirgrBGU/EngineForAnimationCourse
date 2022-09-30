#pragma once

#include <iostream>
#include "../external/eigen/Eigen/Core"
#include "Mesh.h"
#include "Material.h"
#include "Movable.h"
#include "ViewerData.h"

using namespace std;

class Shape : public Movable
{
public:

    static shared_ptr<Shape> Create(const string& file, const shared_ptr<const Material>& material, const shared_ptr<Movable>& parent);

    static shared_ptr<Shape> Create(const string& name, const string& file, const shared_ptr<const Material>& material, const shared_ptr<Movable>& parent);

    static shared_ptr<Shape> Create(const shared_ptr<const Mesh>& mesh, const shared_ptr<const Material>& material, const shared_ptr<Movable>& parent);

    static shared_ptr<Shape> Create(const string& name, const shared_ptr<const Mesh>& mesh, const shared_ptr<const Material>& material, const shared_ptr<Movable>& parent);

    inline auto GetMesh() const { return mesh; }

    inline auto GetMaterial() const { return material; };

    inline void SetShowWireframe(bool show = true) { showWireframe = show; }

    inline bool IsShowWireframe() const { return showWireframe; }

    // TODO: TAL: stuff from ViewerData - need to decide what to do with them

    void AddViewport(int viewport) { viewports = viewports | (1 << viewport); } // TODO: TAL: maybe move
    void RemoveViewport(int viewport) { viewports = viewports & ~(1 << viewport); } // TODO: TAL: maybe move
    bool Is2Render(int viewport) { return (viewports & (1 << viewport)); } // TODO: TAL: maybe move

    ViewerData viewerData;

    inline ViewerData* GetViewerData() override { return &viewerData; };

    unsigned int viewports;

    void Draw(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& norm, int viewportIndex, unsigned int flags) override;
    void DrawForPicking(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& norm, int viewportIndex, PickingData* pickingData) override;

    // helper functions
    Matrix4f CalculateModelTransform(const Matrix4f& view, const Matrix4f& norm);
    void ViewerDataUpdateAndBindMesh(shared_ptr<const Program> program);

private:

    // private constructor (use factory method to create shapes)
    Shape(const string& name, const shared_ptr<const Mesh>& _mesh, const shared_ptr<const Material>& material);

    const shared_ptr<const Mesh> mesh;
    const shared_ptr<const Material> material;

    // TODO: TAL: handle the colors...
    RowVector4f ambient = RowVector4f(1.0, 1.0, 1.0, 1.0);
    RowVector4f diffuse = RowVector4f(1.0, 1.0, 1.0, 1.0);
    RowVector4f specular = RowVector4f(1.0, 1.0, 1.0, 1.0);

	static int pickingIndex; // if pickingIndex >= 0 then we are in picking mode
	static vector<shared_ptr<Shape>> pickingShapes;

    bool showWireframe = false;
};
