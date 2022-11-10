#pragma once

#include <vector>
#include "Viewport.h"
#include "DrawVisitor.h"
#include "GLFW/glfw3.h"


namespace cg3d
{

class Renderer
{
public:
    void AddViewport(const std::shared_ptr<Scene>& scene);
    void AddViewport(const std::shared_ptr<Viewport>& viewport);
    void RemoveViewport(const std::shared_ptr<Viewport>& remove);

    void RenderViewport(Viewport* viewport);
    void RenderViewport(Viewport* viewport, Visitor* visitor);
    void RenderViewportAtPos(int x, int y, Visitor* visitor = nullptr);
    void RenderAllViewports();
    void Draw() { RenderAllViewports(); } // for clarity

    void MouseCallback(int x, int y, int button, int action, int mods, int buttonState[]);
    void ScrollCallback(int x, int y, int xoffset, int yoffset, int buttonState[]);
    void CursorPosCallback(int x, int y, int buttonStates[]);
    void KeyCallback(int x, int y, int key, int scancode, int action, int mods);
    void CharCallback(int x, int y, unsigned int codepoint);
    void WindowSizeCallback(int newWidth, int newHeight);

    [[nodiscard]] inline int GetWindowWidth() const { return windowWidth; }
    [[nodiscard]] inline int GetWindowHeight() const { return windowHeight; };

private:
    Viewport* FindViewportAtPos(int x, int y);

    DrawVisitor defaultVisitor;
    std::vector<std::shared_ptr<Viewport>> viewports;
    Viewport* viewportAtMousePress = nullptr;
    Viewport* viewportAtKeyPress = nullptr;
    int windowWidth = 0, windowHeight = 0, xPress = 0, yPress = 0;
    bool isMouseDragging = false;
};

} // namespace cg3d
