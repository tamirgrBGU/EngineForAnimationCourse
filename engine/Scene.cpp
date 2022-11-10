#include "Scene.h"

#include <utility>
#include "Camera.h"
#include "PickVisitor.h"
#include "Renderer.h"
#include "GLFW/glfw3.h"
#include "Utility.h"


namespace cg3d
{

Scene::Scene(std::string name, Display* display) : Movable(std::move(name)), renderer(display->renderer)
{
    window = display->window;
}

void Scene::Init(Visitor* visitor)
{
    visitor->Init();
}

void Scene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    program.SetUniformMatrix4f("Proj", &proj);
    program.SetUniformMatrix4f("View", &view);
    program.SetUniformMatrix4f("Model", &model);
}

void Scene::MouseCallback(Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[])
{
    // note: there's a (small) chance the button state here precedes the mouse press/release event

    if (action == GLFW_PRESS) { // default mouse button press behavior
        PickVisitor visitor;
        visitor.Init();
        renderer->RenderViewportAtPos(x, y, &visitor); // pick using fixed colors hack
        auto modelAndDepth = visitor.PickAtPos(x, renderer->GetWindowHeight() - y);
        renderer->RenderViewportAtPos(x, y); // draw again to avoid flickering
        pickedModel = modelAndDepth.first ? std::dynamic_pointer_cast<Model>(modelAndDepth.first->shared_from_this()) : nullptr;
        pickedModelDepth = modelAndDepth.second;
        camera->GetRotation().transpose();
        xAtPress = x;
        yAtPress = y;

        if (pickedModel)
            debug("found ", pickedModel->isPickable ? "pickable" : "non-pickable", " model at pos ", x, ", ", y, ": ",
                  pickedModel->name, ", depth: ", pickedModelDepth);
        else
            debug("found nothing at pos ", x, ", ", y);

        if (pickedModel && !pickedModel->isPickable)
            pickedModel = nullptr; // for non-pickable models we need only pickedModelDepth for mouse movement calculations later

        if (pickedModel)
            pickedToutAtPress = pickedModel->GetTout();
        else
            cameraToutAtPress = camera->GetTout();
    }
}

void Scene::ScrollCallback(Viewport* viewport, int x, int y, int xoffset, int yoffset, bool dragging, int buttonState[])
{
    // note: there's a (small) chance the button state here precedes the mouse press/release event
    auto system = camera->GetRotation().transpose();
    if (pickedModel) {
        pickedModel->TranslateInSystem(system, {0, 0, -float(yoffset)});
        pickedToutAtPress = pickedModel->GetTout();
    } else {
        camera->TranslateInSystem(system, {0, 0, -float(yoffset)});
        cameraToutAtPress = camera->GetTout();
    }
}

void Scene::CursorPosCallback(Viewport* viewport, int x, int y, bool dragging, int* buttonState)
{
    if (dragging) {
        auto system = camera->GetRotation().transpose();
        auto moveCoeff = camera->CalcMoveCoeff(pickedModelDepth, viewport->width);
        auto angleCoeff = camera->CalcAngleCoeff(viewport->width);
        if (pickedModel) {
            pickedModel->SetTout(pickedToutAtPress);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE)
                pickedModel->TranslateInSystem(system, {float(x - xAtPress) / moveCoeff, float(yAtPress - y) / moveCoeff, 0});
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                pickedModel->RotateInSystem(system, float(x - xAtPress) / moveCoeff, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE) {
                pickedModel->RotateInSystem(system, float(x - xAtPress) / moveCoeff, Axis::Y);
                pickedModel->RotateInSystem(system, float(y - yAtPress) / moveCoeff, Axis::X);
            }
        } else {
            camera->SetTout(cameraToutAtPress);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE)
                camera->TranslateInSystem(system, {float(xAtPress - x) / moveCoeff, float(y - yAtPress) / moveCoeff, 0});
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                camera->RotateInSystem(system, float(x - xAtPress) / 180, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE) {
                camera->RotateInSystem(system, float(x - xAtPress) / angleCoeff, Axis::Y);
                camera->RotateInSystem(system, float(y - yAtPress) / angleCoeff, Axis::X);
            }
        }
    }
}

void Scene::KeyCallback(Viewport* viewport, int x, int y, int key, int scancode, int action, int mods)
{
    auto system = camera->GetRotation().transpose();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) // NOLINT(hicpp-multiway-paths-covered)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_UP:
                camera->RotateInSystem(system, 0.1f, Axis::X);
                break;
            case GLFW_KEY_DOWN:
                camera->RotateInSystem(system, -0.1f, Axis::X);
                break;
            case GLFW_KEY_LEFT:
                camera->RotateInSystem(system, 0.1f, Axis::Y);
                break;
            case GLFW_KEY_RIGHT:
                camera->RotateInSystem(system, -0.1f, Axis::Y);
                break;
            case GLFW_KEY_W:
                camera->TranslateInSystem(system, {0, 0.05f, 0});
                break;
            case GLFW_KEY_S:
                camera->TranslateInSystem(system, {0, -0.05f, 0});
                break;
            case GLFW_KEY_A:
                camera->TranslateInSystem(system, {-0.05f, 0, 0});
                break;
            case GLFW_KEY_D:
                camera->TranslateInSystem(system, {0.05f, 0, 0});
                break;
            case GLFW_KEY_B:
                camera->TranslateInSystem(system, {0, 0, 0.05f});
                break;
            case GLFW_KEY_F:
                camera->TranslateInSystem(system, {0, 0, -0.05f});
                break;
        }
    }
}

void Scene::CharCallback(Viewport* viewport, int x, int y, unsigned int codepoint)
{
    debug("char codepoint input: ", codepoint, " ('", (char) codepoint, "')");
}

void Scene::ViewportSizeCallback(Viewport* viewport)
{
    camera->SetProjection(float(viewport->width) / float(viewport->height));
}

} // namespace cg3d
