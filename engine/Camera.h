#pragma once

#include "Movable.h"
#include <Eigen/Core>


namespace cg3d
{

// igl programmers are idiots...
#undef far
#undef near

class Camera : virtual public Movable
{
public:

    Camera(std::string name, double fov, double widthHeightRatio, double near, double far);
    ~Camera() override = default;

    inline const Eigen::Matrix4f& GetViewProjection() const { return projection; }

    void SetProjection(double widthHeightRatio);
    float CalcMoveCoeff(float depth, int width) const;

    float CalcAngleCoeff(int width) const;
    Eigen::Matrix4f projection{Eigen::Matrix4f::Identity()};
    double fov = 0, far = 0, near = 0, length = 0, ratio = 0;
};

} // namespace cg3d
