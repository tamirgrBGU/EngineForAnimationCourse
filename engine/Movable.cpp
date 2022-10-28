#include "Movable.h"
#include "Visitor.h"
#include <iostream>
#include <memory>


namespace cg3d
{

Movable::Movable(const Movable& other) : enable_shared_from_this(other), name(other.name + " copy"), isStatic(other.isStatic), isPickable(other.isPickable),
        lineWidth(other.lineWidth), aggregatedTransform(other.aggregatedTransform), Tin(other.Tin), Tout(other.Tout)
{
    // todo: copy the children
}

std::shared_ptr<Movable> Movable::Create(std::string name, const std::shared_ptr<Movable>& parent)
{
    auto movable = std::make_shared<Movable>(std::move(name));
    movable->SetParent(parent);
    return movable;
}

const Eigen::Vector3f& Movable::AxisVec(Axis axis)
{
    static const Eigen::Vector3f AxisVecX = Eigen::Vector3f::UnitX();
    static const Eigen::Vector3f AxisVecY = Eigen::Vector3f::UnitY();
    static const Eigen::Vector3f AxisVecZ = Eigen::Vector3f::UnitZ();
    static const Eigen::Vector3f AxisVecAll = Eigen::Vector3f(1.0f, 1.0f, 1.0f).normalized();

    return axis == Axis::X ? AxisVecX : axis == Axis::Y ? AxisVecY : axis == Axis::Z ? AxisVecZ : AxisVecAll;
}

void Movable::PropagateTransform() // NOLINT(misc-no-recursion)
{
    if (auto p = parent.lock()) // use the aggregatedTransform of the parent
        aggregatedTransform = p->aggregatedTransform * Tout.matrix() * Tin.matrix();
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
            axis == Axis::X || axis == Axis::All ? dist : 0.0f,
            axis == Axis::Y || axis == Axis::All ? dist : 0.0f,
            axis == Axis::Z || axis == Axis::All ? dist : 0.0f
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
    Rotate(angle, AxisVec(axis));
}

void Movable::Rotate(float angle, const Eigen::Vector3f& axisVec)
{
    if (isStatic) return;
    Tout.rotate(Eigen::AngleAxisf(angle, axisVec));
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
            axis == Axis::X || axis == Axis::All ? factor : 1.0f,
            axis == Axis::Y || axis == Axis::All ? factor : 1.0f,
            axis == Axis::Z || axis == Axis::All ? factor : 1.0f
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
    Scale(factor, Axis::All);
}

void Movable::SetParent(const std::shared_ptr<Movable>& newParent, bool retransform)  // TODO: TAL: newParent shouldn't be a shared_ptr?
{
    const std::shared_ptr<Movable>& oldParent = parent.lock();
    if (oldParent != newParent) {
        auto self = this;
        auto compareToThis = [self](std::shared_ptr<Movable> const& m) { return m.get() == self; };
        if (oldParent != nullptr) { // detach from current parent if exists
            auto it = std::find_if(oldParent->children.begin(), oldParent->children.end(), compareToThis);
            if (it != oldParent->children.end())
                oldParent->children.erase(it);
        }
        parent = newParent;
        if (newParent) { // attach to new parent if exists and not already attached
            if (std::find_if(newParent->children.begin(), newParent->children.end(), compareToThis) == newParent->children.end()) {
                newParent->children.emplace_back(shared_from_this());
                if (retransform) { // calculate current translation/rotation in relation to the new parent
                    Tout.matrix() = newParent->aggregatedTransform.inverse() * aggregatedTransform;
                    Tin = Eigen::Affine3f::Identity();
                }
                PropagateTransform();
            }
        }
    }
}

void Movable::Accept(Visitor* visitor) // NOLINT(misc-no-recursion)
{
    for (const auto& child: children)
        child->Accept(visitor);
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
    return aggregatedTransform;
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
