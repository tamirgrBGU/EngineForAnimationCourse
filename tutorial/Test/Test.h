#pragma once

#pragma warning(push, 3)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)

#include "Viewer.h"
#include "Shape.h"

#pragma warning(pop)

using Eigen::Matrix4f;

class Test : public Viewer
{
private:

    shared_ptr<Shape> cubes[3][3][3];
    shared_ptr<Movable> invisible;

public:

    Test() : Viewer("Game2Viewer") {}
    void Init() override;
    void Update(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& model, const shared_ptr<const Program>& program) override;
};

