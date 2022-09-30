#pragma once

#include "MeshGL.h"

#include "Movable.h"
#include "Material.h"
#include "glfw/Viewer.h"
#include "Camera.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <utility>
#include <vector>
#include <string>
#include <cstdint>

#define IGL_MOD_SHIFT           0x0001
#define IGL_MOD_CONTROL         0x0002
#define IGL_MOD_ALT             0x0004
#define IGL_MOD_SUPER           0x0008



class Scene : public Movable, public igl::opengl::glfw::Viewer // TODO: get rid of the multiple inheritance
{
public:
    enum transformations
    {
        xTranslate, yTranslate, zTranslate, xRotate, yRotate, zRotate, xScale, yScale, zScale, scaleAll, reset
    };
    enum modes
    {
        POINTS, LINES, LINE_LOOP, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, QUADS
    };
    enum buffers
    {
        COLOR, DEPTH, STENCIL, BACK, FRONT, NONE
    };

    virtual void Animate() {}

    virtual void WhenTranslate() {}

    explicit Scene(const string& name);
    Scene() : Scene("Scene") {}

    ~Scene() override = default;

    virtual void UpdateOverlay(const Matrix4f &Proj, const Matrix4f &View, const Matrix4f &Model, unsigned int shapeIndex, bool is_points);

//    inline bool IsActive() const { return isActive; }

//    inline void Activate() { isActive = true; }

//    inline void Deactivate() { isActive = false; }

//    vector<int> pShapes;
//    vector<Texture *> textures;
//    Vector3f pickedNormal;

    shared_ptr<Movable> pickedObject = nullptr;

    unsigned int staticScene;

    Program *overlay_program;
    Program *overlay_point_program;

    // Keep track of the global position of the scroll wheel
    float scroll_position;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    void Draw(int viewport, const shared_ptr<Camera>& camera, unsigned int flags);
    void DrawForPicking(int viewport, const shared_ptr<Camera>& camera, PickingData* pickingData);

    void ClearPickedShapes(int viewportIndex);

    float AddPickedShapes(const Matrix4f &PV, const Vector4i &viewport, int viewportIndex, int left, int right, int up, int bottom, int newViewportIndx);

    void MouseProcessing(int button, int xDiff, int yDiff, float depth, int width, const shared_ptr<Camera>& camera) const;

    virtual void WhenTranslate(float dx, float dy) {}

//    virtual void WhenRotate(float dx, float dy) {}

    virtual void WhenScroll(float dy) {}

    virtual void WhenTranslate(const Matrix4f& preMat, float dx, float dy);

    virtual void WhenScroll(const Matrix4f &preMat, float dy);

//    virtual void WhenRotate(const Matrix4f &preMat, float dx, float dy);

    void Pick(const unsigned char data[4], PickingData *pickingData);

    inline void UnPick() { selected_data_index = -1; }
};
