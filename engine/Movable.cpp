#include "Movable.h"
#include "Visitor.h"
#include <iostream>
#include <memory>


namespace cg3d
{

Movable::Movable(const Movable& other) // NOLINT(misc-no-recursion)
        : enable_shared_from_this(other), name(other.name + " copy"), isStatic(other.isStatic), isPickable(other.isPickable),
          lineWidth(other.lineWidth), aggregatedTransform(other.aggregatedTransform), Tin(other.Tin), Tout(other.Tout)
{
    for (const auto& child: other.children)
        children.emplace_back(child->Clone());
}

Movable& Movable::operator=(const Movable& other)
{
    if (this != &other) {
        name = other.name + "copy";
        isStatic = other.isStatic;
        isPickable = other.isPickable;
        lineWidth = other.lineWidth;
        aggregatedTransform = other.aggregatedTransform;
        Tin = other.Tin;
        Tout = other.Tout;
    }

    for (const auto& child: other.children)
        children.emplace_back(child->Clone());

    return *this;
}

std::shared_ptr<Movable> Movable::Clone() // NOLINT(misc-no-recursion)
{
    auto clone{std::shared_ptr<Movable>(new Movable{*this})};
    if (auto p = parent.lock())
        p->AddChild(clone);
    return clone;
}

void Movable::AddChild(std::shared_ptr<Movable> child)
{
    // calling this method when already attached to the parent will push the child to the end
    RemoveChild(child);
    child->parent = shared_from_this();
    children.emplace_back(std::move(child));
}

void Movable::AddChildren(const std::vector<std::shared_ptr<Movable>>& _children)
{
    for (const auto& child: _children)
        AddChild(child);
}

void Movable::RemoveChild(const std::shared_ptr<Movable>& child)
{
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end())
        children.erase(it);
    child->parent.reset();
}

const Eigen::Vector3f& Movable::AxisVec(Axis axis)
{
    static const Eigen::Vector3f vec[]{
            Eigen::Vector3f::UnitX(),
            Eigen::Vector3f::UnitY(),
            Eigen::Vector3f::UnitZ(),
            Eigen::Vector3f::UnitX() + Eigen::Vector3f::UnitY(),
            Eigen::Vector3f::UnitX() + Eigen::Vector3f::UnitZ(),
            Eigen::Vector3f::UnitY() + Eigen::Vector3f::UnitZ(),
            Eigen::Vector3f::UnitX() + Eigen::Vector3f::UnitY() + Eigen::Vector3f::UnitZ()
    };

    return vec[static_cast<int>(axis)];
}

void Movable::PropagateTransform() // NOLINT(misc-no-recursion)
{
    if (auto p = parent.lock()) // use the aggregatedTransform of the parent
        aggregatedTransform = p->aggregatedTransform * Tout.matrix() * Eigen::Affine3f{Eigen::Translation3f(Tin.translation())}.matrix();
    else // there is no parent 
        aggregatedTransform = Tout.matrix() * Tin.matrix();
    
    for (const auto& child: children)
        child->PropagateTransform();
}

void Movable::SetCenter(const Eigen::Vector3f& point)
{
    Tout.pretranslate(point);
    Tin.pretranslate(-point);
    PropagateTransform();
}

void Movable::Translate(const Eigen::Vector3f& vec)
{
    if (isStatic) return;
    Tout.pretranslate(vec);
    PropagateTransform();
}

void Movable::Translate(float dist, Axis axis)
{
    if (isStatic) return;
    Eigen::Vector3f vec(
            axis == Axis::X || axis == Axis::XYZ ? dist : 0.0f,
            axis == Axis::Y || axis == Axis::XYZ ? dist : 0.0f,
            axis == Axis::Z || axis == Axis::XYZ ? dist : 0.0f
    );

    Translate(vec);
}

void Movable::TranslateInSystem(const Eigen::Matrix3f& system, const Eigen::Vector3f& vec)
{
    if (isStatic) return;
    Tout.pretranslate(system.transpose() * vec);
    PropagateTransform();
}

Eigen::Vector3f Movable::GetTranslation() const
{
    return Eigen::Affine3f(aggregatedTransform).translation();
}

void Movable::Rotate(const Eigen::Matrix3f& rot)
{
    if (isStatic) return;
    Tout.rotate(rot);
    PropagateTransform();
}

void Movable::Rotate(float angle, Axis axis)
{
    Rotate(angle, AxisVec(axis).normalized());
}

void Movable::Rotate(float angle, const Eigen::Vector3f& axisVec)
{
    if (isStatic) return;
    Tout.rotate(Eigen::AngleAxisf(angle, axisVec.normalized()));
    PropagateTransform();
}

void Movable::RotateByDegree(float degree, Axis axis)
{
    RotateByDegree(degree, AxisVec(axis));
}

void Movable::RotateByDegree(float degree, const Eigen::Vector3f& axisVec)
{
    if (isStatic) return;
    Tout.rotate(Eigen::AngleAxisf(degree * float(PI_DIV_180), axisVec));
    PropagateTransform();
}

void Movable::RotateInSystem(const Eigen::Matrix3f& system, float angle, Axis axis)
{
    if (isStatic) return;
    Tout.rotate(Eigen::AngleAxisf(angle, Tout.rotation().transpose() * system.transpose() * AxisVec(axis)));
    PropagateTransform();
}

Eigen::Matrix3f Movable::GetRotation() const
{
    return Eigen::Affine3f(aggregatedTransform).rotation();
}

void Movable::Scale(float factor, Axis axis)
{
    if (abs(factor) < 1e-5) return; // factor is too small

    Eigen::Vector3f scaleVec(
            axis == Axis::X || axis == Axis::XYZ ? factor : 1.0f,
            axis == Axis::Y || axis == Axis::XYZ ? factor : 1.0f,
            axis == Axis::Z || axis == Axis::XYZ ? factor : 1.0f
    );

    Scale(scaleVec);
}

void Movable::Scale(const Eigen::Vector3f& scaleVec)
{
    if (isStatic) return;
    Tin.scale(scaleVec);
    PropagateTransform();
}

void Movable::Scale(float factor)
{
    Scale(factor, Axis::XYZ);
}

Eigen::Affine3f Movable::GetRotation(const Eigen::Matrix4f& _transform)
{
    return Eigen::Affine3f{Eigen::Affine3f(_transform).rotation()};
}

Eigen::Affine3f Movable::GetTranslation(const Eigen::Matrix4f& _transform)
{
    return Eigen::Affine3f{Eigen::Translation3f(Eigen::Affine3f(_transform).translation())};
}

Eigen::Affine3f Movable::GetTranslationRotation(const Eigen::Matrix4f& _transform)
{
    return GetTranslation(_transform) * GetRotation(_transform);
}

Eigen::Affine3f Movable::GetScaling(const Eigen::Matrix4f& _transform)
{
    float Sx = _transform.block<3, 1>(0, 0).norm();
    float Sy = _transform.block<3, 1>(0, 1).norm();
    float Sz = _transform.block<3, 1>(0, 2).norm();

    return Eigen::Affine3f{Eigen::Scaling(Sx, Sy, Sz)};
}

Eigen::Matrix4f Movable::GetAggregatedTransform() const
{
    if (auto p = parent.lock())
        return aggregatedTransform * GetScaling(Tin.matrix()).matrix();
    else
        return Tout.matrix() * Tin.matrix();
}

Eigen::Affine3f Movable::GetTin() const
{
    return Tin;
}

Eigen::Affine3f Movable::GetTout() const
{
    return Tout;
}

void Movable::SetTin(const Eigen::Affine3f& newTin)
{
    Tin = newTin;
    PropagateTransform();
}

void Movable::SetTout(const Eigen::Affine3f& newTout)
{
    Tout = newTout;
    PropagateTransform();
}

void Movable::SetTinTout(const Eigen::Affine3f& newTin, const Eigen::Affine3f& newTout)
{
    Tin = newTin;
    Tout = newTout;
    PropagateTransform();
}

Eigen::Matrix4f Movable::GetTransform()
{
    return Tout.matrix() * Tin.matrix();
}

void Movable::SetTransform(const Eigen::Matrix4f& transform)
{
    SetTinTout(GetScaling(transform), GetTranslationRotation(transform));
}

} // namespace cg3d
