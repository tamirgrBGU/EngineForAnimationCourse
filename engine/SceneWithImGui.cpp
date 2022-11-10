#include "SceneWithImGui.h"

#include <utility>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "GLFW/glfw3.h"
#include "Display.h"


namespace cg3d
{

SceneWithImGui::SceneWithImGui(std::string name, Display* display) : Scene(std::move(name), display)
{
    // setting up Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
}

void SceneWithImGui::Accept(Visitor* visitor)
{
    Scene::Accept(visitor);
    DrawImGui();
}

void SceneWithImGui::DrawImGui()
{
    glDisable(GL_DEPTH_TEST);

    // setting up a new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // build and render the widgets
    BuildImGui();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SceneWithImGui::MouseCallback(Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[])
{
    if (ImGui::GetIO().WantCaptureMouse) return;
    Scene::MouseCallback(viewport, x, y, button, action, mods, buttonState);
}

void SceneWithImGui::ScrollCallback(Viewport* viewport, int x, int y, int xoffset, int yoffset, bool dragging, int buttonState[])
{
    if (ImGui::GetIO().WantCaptureMouse) return;
    Scene::ScrollCallback(viewport, x, y, xoffset, yoffset, dragging, buttonState);
}

void SceneWithImGui::CursorPosCallback(Viewport* viewport, int xNew, int yNew, bool dragging, int* buttonState)
{
    if (ImGui::GetIO().WantCaptureMouse) return;
    Scene::CursorPosCallback(viewport, xNew, yNew, dragging, buttonState);
}

void SceneWithImGui::KeyCallback(Viewport* viewport, int x, int y, int key, int scancode, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureKeyboard) return;
    Scene::KeyCallback(viewport, x, y, key, scancode, action, mods);
}

void SceneWithImGui::CharCallback(Viewport* viewport, int x, int y, unsigned int codepoint)
{
    if (ImGui::GetIO().WantCaptureKeyboard) return;
    Scene::CharCallback(viewport, x, y, codepoint);
}

} // namespace cg3d
