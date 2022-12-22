#pragma once

#include "Scene.h"
#include "AABB.h"

#include <utility>

class BasicScene : public cg3d::Scene
{
public:
    explicit BasicScene(std::string name, cg3d::Display* display) : Scene(std::move(name), display) {};
    void Init(float fov, int width, int height, float near, float far);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;


private:

    std::shared_ptr<Movable> root;
    std::shared_ptr<cg3d::Model> camel, sphere1;
    igl::AABB<Eigen::MatrixXd, 3> leftTree;
    igl::AABB<Eigen::MatrixXd, 3> rightTree;

    void CheckAndHandleCollusion();
    void HandleCollusion(std::shared_ptr<cg3d::Model> model1, std::shared_ptr<cg3d::Model> model2);

};
