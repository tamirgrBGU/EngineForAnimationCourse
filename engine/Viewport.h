#pragma once

#include <memory>
#include <utility>
#include <string>


namespace cg3d
{

class Scene;
class Camera;
class Visitor;

struct Viewport : public std::enable_shared_from_this<Viewport>
{
    std::string name;
    int x, y, width, height;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Visitor> visitor; // default visitor for drawing

    Viewport(std::string name, int x, int y, int width, int height, std::shared_ptr<Scene> scene = nullptr)
            : name(std::move(name)), x(x), y(y), width(width), height(height), scene(std::move(scene)) {}

    void Bind() const;
    bool IsInside(int _x, int _y) const;
    void Resize(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // namespace cg3d
