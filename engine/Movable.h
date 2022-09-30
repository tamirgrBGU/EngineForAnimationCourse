#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <set>
#include <utility>
#include "ViewerData.h"
#include "Program.h"

using namespace Eigen;
using namespace std;
using igl::opengl::ViewerData;

// helper macro to create an object with the variable name as the object name
#define NewNamedObject(name, creator, ...) auto name = creator(#name, ##__VA_ARGS__)
#define SetNamedObject(name, creator, ...) name = creator(#name, ##__VA_ARGS__)

class Movable : public enable_shared_from_this<Movable>
{
public:
    typedef struct {
        int id; // the shape id (translated to a color) is also the index (+ 1) of the object in the vector
        vector<shared_ptr<Movable>> objects;
    } PickingData;

public:
    const string name;
    enum class Axis { X, Y, Z, All, Reset }; // TODO: TAL: make sense... (separate to 2 enums?)

    static shared_ptr<Movable> Create(const string& name, const shared_ptr<Movable>& parent);

    virtual ~Movable() = default;
    inline const set<shared_ptr<Movable>>& GetChildren() const { return children; };
    virtual ViewerData* GetViewerData() { return nullptr; };
    Matrix4f MakeTransf();
    void TranslateInSystem(const Matrix3f& rot, const Vector3f& point);
    Matrix3f GetRotation() const;
    float GetRotationAngle(Axis axis) const;
    void SetCenter(const Vector3f& point);
    void RotateInSystem(float angle, Axis axis);

    void SetParent(const shared_ptr<Movable>& newParent, bool retransform = false);
    void Translate(float dist, Axis axis);
    void Translate(const Vector3f& vec);
    void Rotate(float angle, Axis axis);
    void Rotate(float angle, const Vector3f& axisVec);
    void Rotate(const Matrix3f& rot);
    void Scale(float factor, Axis axis = Axis::All);
    void Scale(const Vector3f& scaleVec);

    Movable(const Movable& t) = delete; // copy constructor not allowed
    inline void SetStatic(bool _isStatic = true) { isStatic = _isStatic; };
    inline void SetPickable(bool _isPickable = true) { isPickable = _isPickable; };
    inline void SetInverseViewTransform(bool _inverseCameraMovement = true) { inverseViewTransform = _inverseCameraMovement; }
    inline const Matrix4f& GetTransform() { return transform; };
    virtual void Draw(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& normal, int viewportIndex, unsigned int flags);
    virtual void DrawForPicking(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& normal, int viewportIndex, PickingData* pickingData);
    virtual void Update(const Matrix4f &proj, const Matrix4f &view, const Matrix4f &model, const shared_ptr<const Program>& program);

public:

    explicit Movable(string name) : name(std::move(name)), Tout(Affine3f::Identity()), Tin(Affine3f::Identity()), transform(Matrix4f::Identity()) {}

    Affine3f Tout, Tin;
    bool inverseViewTransform = false;
    bool isPickable = true;
    bool isStatic = false;
    set<shared_ptr<Movable>> children;
    Matrix4f transform;

private:

    void UpdateTransforms();

    weak_ptr<Movable> parent;

    static const Vector3f& AxisVec(Axis axis); // helper
};
