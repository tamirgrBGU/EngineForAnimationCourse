#include "Camera.h"
#include "frustum.h"
#include "ortho.h"
#include "PI.h"
#include <iostream>

shared_ptr<Camera> Camera::Create(const string& name, float fov, float relationWH, float near, float far, const shared_ptr<Movable>& parent)
{
    auto camera = shared_ptr<Camera>(new Camera(name, fov, relationWH, near, far));
    camera->SetParent(parent);
    return camera;
}

Camera::Camera(const string& name, float fov, float relationWH, float near, float far) : Movable(name), fov(fov), near(near), far(far)
{
    this->length = 2.0f;
    this->near = near;
    this->far = far;
    SetProjection(fov, relationWH);
}

void Camera::SetProjection(float _fov, float relationWH)
{
    if (_fov > 0) {
        float fH = float(tan(_fov / 360.0 * igl::PI)) * near;
        float fW = fH * relationWH;
        igl::frustum(-fW, fW, -fH, fH, near, far, projection);
        fov = _fov;
    } else {
        float camera_view_angle = 45.0;
        float h = float(tan(camera_view_angle / 360.0 * igl::PI)) * (length);
        igl::ortho(-h * relationWH, h * relationWH, -h, h, near, far, projection);
    }
}

float Camera::CalcMoveCoeff(float depth, int width) const
{
    float z = far + depth * (near - far);
    if (fov > 0)
        return float(double(width) / far * z * near / 2.0 / tan(fov / 360.0 * igl::PI)); // / camera z translate;
    else
        return float(0.5 * width);
}

void Camera::Draw(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& normal, int viewportIndex, unsigned int flags)
{
    // TODO: TAL
}

