#pragma once

#include "Scene.h"

#include <utility>
#include "Connector.h"
#include <stack>
class BasicScene : public cg3d::Scene
{
public:
    explicit BasicScene(std::string name, cg3d::Display* display) : Scene(std::move(name), display) {};
    void Init(float fov, int width, int height, float near, float far);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
    void KeyCallback(cg3d::Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods) override;


private:
    std::shared_ptr<Movable> root;
    std::shared_ptr<cg3d::Model> cyl, sphere1 ,cube;
    std::unordered_map<std::string, std::stack<std::vector<std::shared_ptr<cg3d::Mesh>>>> previousMeshLists;
    void decreaseQuality();
    void increaseQuality();

};
