#pragma once

#include <memory>


namespace cg3d
{

class Scene;
class Camera;
class Visitor;

class Viewport : public std::enable_shared_from_this<Viewport>
{
public:

    int x, y, width, height;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Visitor> visitor; // default visitor for drawing

    Viewport(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}

    void Bind() const;
    bool IsInside(int _x, int _y) const;
    void Resize(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // namespace cg3d
