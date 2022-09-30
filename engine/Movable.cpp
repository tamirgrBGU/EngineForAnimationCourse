#include "Movable.h"
#include <iostream>
#include <memory>

shared_ptr<Movable> Movable::Create(const string& name, const shared_ptr<Movable>& parent)
{
    auto movable = std::make_shared<Movable>(name);
    movable->SetParent(parent);
    return movable;
}

const Vector3f& Movable::AxisVec(Axis axis)
{
    static const Vector3f AxisVecX = Vector3f::UnitX();
    static const Vector3f AxisVecY = Vector3f::UnitY();
    static const Vector3f AxisVecZ = Vector3f::UnitZ();
    static const Vector3f AxisVecAll = Vector3f(1.0f, 1.0f, 1.0f).normalized();

    return axis == Axis::X ? AxisVecX : axis == Axis::Y ? AxisVecY : AxisVecZ;
}

void Movable::UpdateTransforms() // NOLINT(misc-no-recursion)
{
    if (auto p = parent.lock()) // use the transform of the parent
        transform = p->transform * Tout.matrix() * Tin.matrix();
    else // there is no parent
        transform = Tout.matrix() * Tin.matrix();

    for (const auto& child: children)
        child->UpdateTransforms();
}

Matrix4f Movable::MakeTransf()
{
    Matrix4f mat = Matrix4f::Identity();
    mat.col(3) << Tin.translation(), 1;
    return Tout.matrix() * mat;
}

void Movable::TranslateInSystem(const Matrix3f& rot, const Vector3f& point) // TODO: TAL: check
{
    Tout.pretranslate(rot.transpose() * point);
    UpdateTransforms();
}

void Movable::SetCenter(const Vector3f& point)
{
    Tout.pretranslate(point);
    Tin.pretranslate(-point);
    UpdateTransforms();
}

void Movable::RotateInSystem(float angle, Axis axis) // TODO: TAL: check
{
    Tout.rotate(AngleAxisf(angle, Tout.rotation().transpose() * AxisVec(axis)));
    UpdateTransforms();
}

void Movable::Rotate(float angle, Axis axis)
{
    Rotate(angle, AxisVec(axis));
}

void Movable::Rotate(float angle, const Vector3f& axisVec) // NOLINT(misc-no-recursion)
{
    if (!isStatic) {
        Tout.rotate(AngleAxisf(angle, axisVec));
        UpdateTransforms();
    }
}

void Movable::Rotate(const Matrix3f& rot) // NOLINT(misc-no-recursion)
{
    if (!isStatic) {
        Tout.rotate(rot);
        UpdateTransforms();
    }
}

void Movable::Translate(const Vector3f& vec) // NOLINT(misc-no-recursion)
{
    if (!isStatic) {
        Tout.pretranslate(vec);
        UpdateTransforms();
    }
}

void Movable::Translate(float dist, Axis axis)
{
    Vector3f vec(
            axis == Axis::X || axis == Axis::All ? dist : 0.0f,
            axis == Axis::Y || axis == Axis::All ? dist : 0.0f,
            axis == Axis::Z || axis == Axis::All ? dist : 0.0f
    );

    Translate(vec);

    //if (preRotation)
    //	Tout.pretranslate(amt);
    //else
    //	Tin.pretranslate(amt);
}

void Movable::Scale(float factor, Axis axis)
{
    if (abs(factor) < 1e-5)
        return; // factor is too small

    Vector3f scaleVec(
            axis == Axis::X || axis == Axis::All ? factor : 1.0f,
            axis == Axis::Y || axis == Axis::All ? factor : 1.0f,
            axis == Axis::Z || axis == Axis::All ? factor : 1.0f
    );

    Scale(scaleVec);
}

void Movable::Scale(const Vector3f& scaleVec) // NOLINT(misc-no-recursion)
{
    if (!isStatic) {
        Tin.scale(scaleVec);
        UpdateTransforms();
    }
}
void Movable::SetParent(const shared_ptr<Movable>& newParent, bool retransform)
{
    const shared_ptr<Movable> &oldParent = parent.lock();
    if (oldParent != newParent) {
        if (oldParent != nullptr) // detach from current parent if exists
            oldParent->children.erase(shared_from_this());
        parent = newParent;
        if (newParent) { // attach to new parent if exists
            newParent->children.insert(shared_from_this());

            if (retransform) { // calculate current translation/rotation in relation to the new parent
                Tout.matrix() = newParent->transform.inverse() * transform;
                Tin = Affine3f::Identity();
            }
            UpdateTransforms();
        }
    }
}

void Movable::Update(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& model, const shared_ptr<const Program>& program) // NOLINT(misc-no-recursion)
{
    const shared_ptr<Movable> &myParent = parent.lock();

    if (myParent)
        myParent->Update(proj, view, model, program);
}

void Movable::Draw(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& norm, int viewportIndex, unsigned int flags) // NOLINT(misc-no-recursion)
{
    for (const auto& child : children)
        child->Draw(proj, view, norm, viewportIndex, flags);
}

void Movable::DrawForPicking(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& norm, int viewportIndex, PickingData *pickingData) // NOLINT(misc-no-recursion)
{
    for (const auto& child : children)
        child->DrawForPicking(proj, view, norm, viewportIndex, pickingData);
}

Matrix3f Movable::GetRotation() const {
    return Tout.rotation();
}

float Movable::GetRotationAngle(Axis axis) const
{
    return 0; // AngleAxisf(Tout.rotation(), AxisVec(axis));
}
