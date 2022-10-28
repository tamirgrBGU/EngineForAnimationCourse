#pragma once

#include "ViewerData.h"
#include "Program.h"
#include "Mesh.h"
#include "Material.h"


namespace cg3d
{

class Visitor;

// helper macro for creating an object with the variable name as the first argument
#define NewNamedObject(name, creator, ...) auto name = creator(#name, ##__VA_ARGS__)
#define SetNamedObject(name, creator, ...) name = creator(#name, ##__VA_ARGS__)

#define PI_DIV_180 0.017453292519943295769

class Movable : public std::enable_shared_from_this<Movable>
{
public:
    std::string name;

    enum class Axis
    {
        X, Y, Z, All, Reset
    }; // TODO: TAL: make sense... (separate to 2 enums?)

    explicit Movable(std::string name) : name(std::move(name)) {}
    Movable(const Movable& other);
    virtual ~Movable() { SetParent(nullptr); };

    void SetParent(const std::shared_ptr<Movable>& m, bool retransform = false);
    static std::shared_ptr<Movable> Create(std::string name, const std::shared_ptr<Movable>& parent);

    virtual void Accept(Visitor* visitor);

    virtual void SetCenter(const Eigen::Vector3f& point);

    virtual void Translate(float dist, Axis axis);
    virtual void Translate(const Eigen::Vector3f& vec);
    virtual void TranslateInSystem(const Eigen::Matrix3f& system, const Eigen::Vector3f& vec);
    virtual Eigen::Vector3f GetTranslation() const;

    virtual void Rotate(const Eigen::Matrix3f& rot);
    virtual void Rotate(float angle, Axis axis);
    virtual void Rotate(float angle, const Eigen::Vector3f& axisVec);
    virtual void RotateByDegree(float degree, Axis axis);
    virtual void RotateByDegree(float degree, const Eigen::Vector3f& axisVec);
    virtual void RotateInSystem(const Eigen::Matrix3f& system, float angle, Axis axis);
    virtual Eigen::Matrix3f GetRotation() const;

    virtual void Scale(float factor);
    virtual void Scale(float factor, Axis axis);
    virtual void Scale(const Eigen::Vector3f& scaleVec);

    inline void SetStatic(bool _isStatic = true) { isStatic = _isStatic; };
    inline void SetPickable(bool _isPickable = true) { isPickable = _isPickable; };

    virtual Eigen::Matrix4f GetAggregatedTransform() const;
    virtual Eigen::Matrix4f GetTransform();
    virtual void SetTransform(const Eigen::Matrix4f& transform);
    virtual void PropagateTransform(); // recursively propagates the transform
    virtual Eigen::Affine3f GetTin() const;
    virtual Eigen::Affine3f GetTout() const;
    virtual void SetTin(const Eigen::Affine3f &newTin);
    virtual void SetTout(const Eigen::Affine3f &newTout);
    virtual void SetTinTout(const Eigen::Affine3f &newTin, const Eigen::Affine3f& newTout);

    // helper functions
    static const Eigen::Vector3f& AxisVec(Axis axis);
    static Eigen::Affine3f GetRotation(const Eigen::Matrix4f& transform);
    static Eigen::Affine3f GetTranslation(const Eigen::Matrix4f& transform);
    static Eigen::Affine3f GetTranslationRotation(const Eigen::Matrix4f& transform);
    static Eigen::Affine3f GetScaling(const Eigen::Matrix4f& _transform);

    Eigen::Matrix4f aggregatedTransform{Eigen::Matrix4f::Identity()}; // aggregation of all transformations starting from top level
    Eigen::Affine3f Tout{Eigen::Affine3f::Identity()}, Tin{Eigen::Affine3f::Identity()}; // transformations of *this* object (only)
    float lineWidth = 2;
    bool isPickable = true;
    bool isStatic = false;
    std::vector<std::shared_ptr<Movable>> children;
    std::weak_ptr<Movable> parent;
};

} // namespace cg3d
