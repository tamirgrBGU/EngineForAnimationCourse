#include "Camera.h"
#include "frustum.h"
#include "ortho.h"
#include "PI.h"
#include <iostream>
#include <utility>


namespace cg3d
{

Camera::Camera(std::string name, double fov, double widthHeightRatio, double near, double far)
        : Movable(std::move(name)), fov(fov), ratio(widthHeightRatio), near(near), far(far), length(2.0f)
{
    SetProjection(ratio);
}

void Camera::SetProjection(double widthHeightRatio)
{
    if (fov > 0) {
        double fH = double(tan(igl::PI * fov / 360.0)) * near;
        double fW = fH * widthHeightRatio;
        igl::frustum(float(-fW),float(fW), float(-fH),float(fH), float(near), float(far), projection);
    } else {
        double camera_view_angle = 45.0;
        double h = double(tan(camera_view_angle / 360.0 * igl::PI)) * length;
        igl::ortho(float(-h * widthHeightRatio), float(h * widthHeightRatio), float(-h), float(h), float(near), float(far), projection);
    }
    ratio = widthHeightRatio;
}

float Camera::CalcMoveCoeff(float depth, int width) const
{
    double z = far + depth * (near - far);
    if (fov > 0)
        return float(double(width) / far * z / near / 2.0 / tan(fov / 360.0 * igl::PI) / ratio);
    else
        return float(0.5 * width);
}

float Camera::CalcAngleCoeff(int width) const
{
    return float(double(width) / fov / PI_DIV_180 / ratio);
}

} // namespace cg3d
