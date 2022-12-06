#pragma once

#include "Scene.h"

#include <utility>

class BasicScene : public cg3d::Scene
{
public:
    explicit BasicScene(std::string name, cg3d::Display* display) : Scene(std::move(name), display) {};
    void Init(float fov, int width, int height, float near, float far);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;

private:
    std::shared_ptr<Movable> root;
    std::shared_ptr<cg3d::Model> camel, sphere1 ,cube;
    void CheckAndHandleCollusion(std::shared_ptr<cg3d::Model> model1, std::shared_ptr<cg3d::Model> model2);
    void HandleCollusion(std::shared_ptr<cg3d::Model> model1, std::shared_ptr<cg3d::Model> model2);
    static std::vector<Eigen::Vector3d> VerticesList(std::shared_ptr<cg3d::Model> model);
};
