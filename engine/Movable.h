#pragma once

#include <memory>

#include "Program.h"
#include "Mesh.h"
#include "Material.h"
#include "Visitor.h"
#include "Eigen/Geometry"

class Visitor;

namespace cg3d
{

#define PI_DIV_180 0.017453292519943295769

class Movable : public std::enable_shared_from_this<Movable>
{
protected:
    explicit Movable(std::string name) : name(std::move(name)) {}
    Movable(const Movable& other); // important: doesn't add itself to the parent's children (object isn't constructed yet)
    Movable(Movable&&) = default; // important: doesn't add itself to the parent's children (object isn't constructed yet)
    Movable& operator=(const Movable& other);
private:    
    Eigen::Matrix4f aggregatedTransform{Eigen::Matrix4f::Identity()}; // aggregation of all transformations starting from top level

public:
    std::string name;

    template<typename... Args>
    static std::shared_ptr<Movable> Create(Args&&... args) { return std::shared_ptr<Movable>(new Movable{std::forward<Args>(args)...}); };

    std::shared_ptr<Movable> Clone();
    inline std::shared_ptr<Movable> ptr() { return shared_from_this(); };
    virtual ~Movable() { if (auto p = parent.lock()) p->RemoveChild(ptr()); };

    enum class Axis { X, Y, Z, XY, XZ, YZ, XYZ };

    virtual void Accept(Visitor* visitor) { visitor->Visit(this); }

    void AddChild(std::shared_ptr<Movable> child);
    void AddChildren(const std::vector<std::shared_ptr<Movable>>& _children);
    void RemoveChild(const std::shared_ptr<Movable>& child);

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
    virtual void SetTin(const Eigen::Affine3f& newTin);
    virtual void SetTout(const Eigen::Affine3f& newTout);
    virtual void SetTinTout(const Eigen::Affine3f& newTin, const Eigen::Affine3f& newTout);

     //helper functions
    static const Eigen::Vector3f& AxisVec(Axis axis);
    static Eigen::Affine3f GetRotation(const Eigen::Matrix4f& transform);
    static Eigen::Affine3f GetTranslation(const Eigen::Matrix4f& transform);
    static Eigen::Affine3f GetTranslationRotation(const Eigen::Matrix4f& transform);
    static Eigen::Affine3f GetScaling(const Eigen::Matrix4f& _transform);

    Eigen::Affine3f Tout{Eigen::Affine3f::Identity()}, Tin{Eigen::Affine3f::Identity()}; // transformations of *this* object (only)
    float lineWidth = 5;
    bool isPickable = true;
    bool isStatic = false;
    std::vector<std::shared_ptr<Movable>> children;
    std::weak_ptr<Movable> parent;
    std::weak_ptr<Movable> scene;
};

} // namespace cg3d
