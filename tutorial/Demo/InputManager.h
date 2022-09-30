#pragma once   // maybe should be static class

#include "Demo.h"
#include "Renderer.h"
#include "GLFW/glfw3.h"
#include "Display.h"
#include "Debug.h"

void glfw_mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto renderer = (Renderer*) glfwGetWindowUserPointer(window);
    if (action == GLFW_PRESS) {
        double x2, y2;
        glfwGetCursorPos(window, &x2, &y2);
        renderer->UpdatePress(x2, y2);
        if (renderer->PickObject((int) x2, (int) y2)) {
            renderer->UpdatePosition(x2, y2);
            if (button == GLFW_MOUSE_BUTTON_LEFT)
                renderer->Pressed();
        } else {
            renderer->UnpickObjects();
        }
    } else {
        renderer->UnpickObjects();
    }
}

void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto renderer = (Renderer*) glfwGetWindowUserPointer(window);
    auto scene = (Demo*) renderer->GetScene();

    if (scene->pickedObject) {
        renderer->UpdateZpos((int) yoffset);
        renderer->MouseProcessing(GLFW_MOUSE_BUTTON_MIDDLE, 0, scene->currentCamera);
    } else {
        scene->currentCamera->Translate(-float(yoffset), Movable::Axis::Z);
    }
}

void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto renderer = (Renderer*) glfwGetWindowUserPointer(window);
    auto scene = (Demo*) renderer->GetScene();

    renderer->UpdatePosition(xpos, ypos);

    int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    int middleButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

    if (renderer->CheckViewport((int) xpos, (int) ypos, 0)) {
        if (leftButtonState == GLFW_PRESS)
            renderer->MouseProcessing(GLFW_MOUSE_BUTTON_LEFT, 0, scene->currentCamera);
        if (rightButtonState == GLFW_PRESS)
            renderer->MouseProcessing(GLFW_MOUSE_BUTTON_RIGHT, 0, scene->currentCamera);
        if (middleButtonState == GLFW_PRESS)
            renderer->MouseProcessing(GLFW_MOUSE_BUTTON_MIDDLE, 0, scene->currentCamera);
    }
}

void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
    auto renderer = (Renderer*) glfwGetWindowUserPointer(window);
    renderer->Resize(window, width, height);
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto renderer = (Renderer*) glfwGetWindowUserPointer(window);
    auto scene = (Demo*) renderer->GetScene();
    auto& camera = scene->currentCamera;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) // NOLINT(hicpp-multiway-paths-covered)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_SPACE:
                scene->SetActive(!scene->IsActive());
                break;
            case GLFW_KEY_UP:
                camera->Rotate(0.05, Movable::Axis::X);
                break;
            case GLFW_KEY_DOWN:
                camera->Rotate(-0.05, Movable::Axis::X);
                break;
            case GLFW_KEY_LEFT:
                camera->Rotate(0.05, Movable::Axis::Y);
                break;
            case GLFW_KEY_RIGHT:
                camera->Rotate(-0.05, Movable::Axis::Y);
                break;
            case GLFW_KEY_W:
                camera->Translate(0.25, Movable::Axis::Y);
                break;
            case GLFW_KEY_S:
                camera->Translate(-0.25, Movable::Axis::Y);
                break;
            case GLFW_KEY_A:
                camera->Translate(-0.25, Movable::Axis::X);
                break;
            case GLFW_KEY_D:
                camera->Translate(0.25, Movable::Axis::X);
                break;
            case GLFW_KEY_B:
                camera->Translate(0.5, Movable::Axis::Z);
                break;
            case GLFW_KEY_F:
                camera->Translate(-0.5, Movable::Axis::Z);
                break;
            case GLFW_KEY_1:
                renderer->SetViewportCamera(0, camera = scene->camera1);
                break;
            case GLFW_KEY_2:
                renderer->SetViewportCamera(0, camera = scene->camera2);
                break;
            case GLFW_KEY_3:
                renderer->SetViewportCamera(0, camera = scene->camera3);
                break;
        }
    }
}

void Init(Display& display)
{
    display.AddKeyCallBack(glfw_key_callback);
    display.AddMouseCallBacks(glfw_mouse_callback, glfw_scroll_callback, glfw_cursor_position_callback);
    display.AddResizeCallBack(glfw_window_size_callback);
    //menu->init(&display);
}
