#include "Renderer.h"
#include "Utility.h"
#include "DrawVisitor.h"
#include "Scene.h"
#include "GLFW/glfw3.h"


namespace cg3d
{

Viewport* Renderer::FindViewportAtPos(int x, int y)
{
    if (x >= 0 && x <= windowWidth && y >= 0 && y <= windowHeight) { // check if the cursor is inside the window
        auto iterator = std::find_if(viewports.begin(), viewports.end(), [x, y](auto& viewport) { return viewport->IsInside(x, y); });
        if (iterator != viewports.end())
            return iterator->get();
        debug("no viewport found at ", x, ", ", y);
    }

    return nullptr;
}

void Renderer::RenderViewport(Viewport* viewport)
{
    RenderViewport(viewport, viewport->visitor.get());
}

void Renderer::RenderViewport(Viewport* viewport, Visitor* visitor)
{
    if (!visitor)
        visitor = &defaultVisitor;

    viewport->Bind();
    visitor->Run(viewport->scene.get(), viewport->camera.get());
}

void Renderer::RenderViewportAtPos(int x, int y, Visitor* visitor)
{
    auto viewport = FindViewportAtPos(x, y);

    if (viewport)
        RenderViewport(viewport, visitor ? visitor : viewport->visitor.get());
}

void Renderer::RenderAllViewports()
{
    defaultVisitor.Init();

    for (auto& viewport: viewports)
        RenderViewport(viewport.get());
}

void Renderer::MouseCallback(int x, int y, int button, int action, int mods, int buttonState[])
{
    if (action == GLFW_PRESS) {
        xPress = x;
        yPress = y;
        viewportAtMousePress = FindViewportAtPos(x, y);
    }

    if (viewportAtMousePress)
        viewportAtMousePress->scene->MouseCallback(viewportAtMousePress, x, y, button, action, mods, buttonState);

    if (action == GLFW_RELEASE)
        isMouseDragging = false;
}

void Renderer::ScrollCallback(int x, int y, int xoffset, int yoffset, int buttonState[])
{
    if (viewportAtMousePress) {
        isMouseDragging = true;
    } else {
        isMouseDragging = false;
        viewportAtMousePress = FindViewportAtPos(x, y);
    }

    if (viewportAtMousePress)
        viewportAtMousePress->scene->ScrollCallback(viewportAtMousePress, x, y, xoffset, yoffset, isMouseDragging, buttonState);
}

void Renderer::CursorPosCallback(int x, int y, int buttonStates[])
{
    auto activeViewport = viewportAtMousePress ? viewportAtMousePress : FindViewportAtPos(x, y);
    if (!activeViewport) return;
    if (!isMouseDragging && (abs(x - xPress) > 3 || abs(y - yPress) > 3)) // ignore tiny mouse movements when selecting
        isMouseDragging = buttonStates[GLFW_MOUSE_BUTTON_LEFT] || buttonStates[GLFW_MOUSE_BUTTON_MIDDLE] || buttonStates[GLFW_MOUSE_BUTTON_RIGHT];
    activeViewport->scene->CursorPosCallback(activeViewport, x, y, isMouseDragging, buttonStates);
}

void Renderer::KeyCallback(int x, int y, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && !viewportAtKeyPress) return;

    // if the cursor is in a viewport area, set it as the target, otherwise select the first viewport
    if (Viewport* viewport; action == GLFW_PRESS && (viewport = FindViewportAtPos(x, y)))
        viewportAtKeyPress = viewport;

    if (!viewportAtKeyPress && !viewports.empty())
        viewportAtKeyPress = viewports[0].get();

    if (viewportAtKeyPress)
        viewportAtKeyPress->scene->KeyCallback(viewportAtKeyPress, x, y, key, scancode, action, mods);
}

void Renderer::CharCallback(int x, int y, unsigned int codepoint)
{
    if (viewportAtKeyPress)
        viewportAtKeyPress->scene->CharCallback(viewportAtKeyPress, x, y, codepoint);
}

void Renderer::WindowSizeCallback(int newWidth, int newHeight)
{
    if (windowWidth > 0 && windowHeight > 0) {
        for (const auto& viewport: viewports) {
            viewport->Resize(windowWidth, windowHeight, newWidth, newHeight);
            viewport->scene->ViewportSizeCallback(viewport.get());
        }
    }

    windowWidth = newWidth;
    windowHeight = newHeight;
}

void Renderer::AddViewport(const std::shared_ptr<Viewport>& viewport) // add scene with default camera and draw visitor
{
    // if given 0 for width or height of the viewport, get them from the current viewport
    if (viewport->width == 0 || viewport->height == 0) {
        int viewportRect[4];
        glGetIntegerv(GL_VIEWPORT, viewportRect);
        if (viewport->width == 0)
            viewport->width = viewportRect[2];
        if (viewport->height == 0)
            viewport->height = viewportRect[3];
    }

    if (!viewport->camera)
        viewport->camera = viewport->scene->camera;

    viewports.emplace_back(viewport);

    viewport->scene->AddViewportCallback(viewport.get());
}

void Renderer::AddViewport(const std::shared_ptr<Scene>& scene) // add scene with default camera and draw visitor
{
    auto viewport = std::make_shared<Viewport>(scene->name + "Viewport", 0, 0, 0, 0);
    viewport->scene = dynamic_pointer_cast<Scene>(scene->shared_from_this());
    AddViewport(viewport);
}

void Renderer::RemoveViewport(const std::shared_ptr<Viewport>& remove)
{
    auto it = std::find_if(viewports.begin(), viewports.end(), [remove](auto& viewport) { return viewport.get() == remove.get(); });
    if (it != viewports.end())
        viewports.erase(it);
}

} // namespace cg3d
