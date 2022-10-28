#pragma once
#include "Scene.h"


namespace cg3d
{

class SceneWithImGui : public Scene
{
public:
    explicit SceneWithImGui(std::string name, Display* display);

    void MouseCallback(Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[]) override;
    void ScrollCallback(Viewport* viewport, int x, int y, int xoffset, int yoffset, bool dragging, int buttonState[]) override;
    void CursorPosCallback(Viewport* viewport, int xNew, int yNew, bool dragging, int* buttonState) override;
    void KeyCallback(Viewport* viewport, int x, int y, int key, int scancode, int action, int mods) override;
    void CharCallback(Viewport* viewport, int x, int y, unsigned int codepoint) override;
    void Accept(Visitor* visitor) override;

protected:
    virtual void BuildImGui() = 0;

private:
    void DrawImGui();
};

} // namespace cg3d
