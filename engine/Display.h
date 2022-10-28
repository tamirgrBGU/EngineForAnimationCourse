#pragma once

#include "Viewport.h"
#include <GLFW/glfw3.h>
#include <string>

#define EXIT_FAILURE 1


namespace cg3d
{

class Renderer;

class Display
{
public:
    std::string name;

    Display(std::string title, int width, int height, Renderer* renderer);
    Display(const Display&) = delete; // disable copy constructor
    ~Display();
    void LaunchRendering(bool loop) const;
    void SwapBuffers() const;
    Renderer* renderer;
    GLFWwindow* window;

private:
    struct MouseButtonsStates
    {
        int state[3];
    };

    // callback event handlers
    static void MouseCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void CharCallback(GLFWwindow* window, unsigned int codepoint);
    static void WindowSizeCallback(GLFWwindow* window, int width, int height);

    // helper functions
    static GLFWwindow* CreateWindow(const std::string& title, int width, int height, bool tryWithDebug);
    static GLFWwindow* SetupWindowAndContext(const std::string& title, int width, int height);
    static void SetupDebugCallback();
    static void PrintVersionInfo(GLFWwindow* window);
    static void SetupCallbacks(GLFWwindow* window);
    static Display::MouseButtonsStates GetMouseButtonsStates(GLFWwindow* window);
};

} // namespace cg3d
