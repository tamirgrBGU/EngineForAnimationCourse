#pragma once

#include "Visitor.h"
#include "Camera.h"

#include <utility>
#include <memory>


namespace cg3d
{

class DrawVisitor : public Visitor
{
public:
    void Run(Scene* scene, Camera* camera) override;
    void Visit(Model* model) override;
    void Visit(Scene* _scene) override;
    void Init() override;
    bool drawOutline = true;
    float outlineLineWidth = 5;

    Eigen::Vector4f outlineLineColor{1, 1, 1, 1};

private:
    void DrawOutline();

    Scene* scene;
};

} // namespace cg3d
