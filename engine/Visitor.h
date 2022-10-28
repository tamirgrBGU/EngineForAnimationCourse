#pragma once

#include "Eigen/Core"


namespace cg3d
{

class Movable;
class Model;
class Scene;
class Camera;

class Visitor
{
public:
    explicit Visitor(Scene* scene) : scene(scene) {}

    virtual void Run(Camera* camera);
    virtual void Init() {};
    virtual void Visit(Scene* _scene) {};
    virtual void Visit(Model* model) {};
    virtual void Visit(Movable* movable) {};

    Eigen::Matrix4f proj;
    Eigen::Matrix4f view;
    Eigen::Matrix4f norm;

protected:
    Scene* scene;
};

} // namespace cg3d
