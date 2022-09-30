#pragma once

#include "Movable.h"
#include <igl_inline.h>
#include <Eigen/Geometry>
#include <Eigen/Core>

class Camera : public Movable
{
public:
    static shared_ptr<Camera> Create(const string& name, float fov, float relationWH, float near, float far, const shared_ptr<Movable>& parent);

    inline const Eigen::Matrix4f& GetViewProjection() const { return projection; }

    void SetProjection(float _fov, float relationWH);
    float CalcMoveCoeff(float depth, int width) const;

    Eigen::Matrix4f projection;
    float fov, far, near, length;

    void Draw(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& normal, int viewportIndex, unsigned int flags) override;

private:
    Camera(const string& name, float fov, float relationWH, float near, float far);
};
