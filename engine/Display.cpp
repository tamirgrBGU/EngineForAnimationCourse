#include <chrono>
#include <thread>
#include <utility>

#include "gl.h"
#include "GLFW/glfw3.h"
#include "Display.h"

#include <get_seconds.h>
#include "Renderer.h"
#include "Utility.h"
#include "DebugHacks.h"


namespace cg3d
{

Display::Display(std::string title, int width, int height, Renderer* renderer)
        : name(std::move(title)), renderer(renderer), window(SetupWindowAndContext(title, width, height))
{
    SetupDebugCallback(); // will only work with opengl 4.3 or higher
    PrintVersionInfo(window);
    SetupCallbacks(window);
    glfwSetWindowUserPointer(window, renderer);
    WindowSizeCallback(window, width, height); // register window size in the renderer
#ifdef _DEBUG
    DebugHacks::Init(window);
#endif
}

void Display::SetupCallbacks(GLFWwindow* window)
{
    glfwSetMouseButtonCallback(window, MouseCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);
}

void Display::PrintVersionInfo(GLFWwindow* window)
{
#if defined(DEBUG) || defined(_DEBUG)
    debug("OpenGL Version ", GLVersion.major, ".", GLVersion.minor, " loaded");
    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    debug("OpenGL version received: ", major, ".", minor, ".", rev);
    debug("Supported OpenGL is ", (const char*) glGetString(GL_VERSION));
    debug("Supported GLSL is ", (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
}

Display::MouseButtonsStates Display::GetMouseButtonsStates(GLFWwindow* window)
{
    MouseButtonsStates states{};

    states.state[GLFW_MOUSE_BUTTON_LEFT] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    states.state[GLFW_MOUSE_BUTTON_MIDDLE] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    states.state[GLFW_MOUSE_BUTTON_RIGHT] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    return states;
}

void Display::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    auto mouseButtonsStates = GetMouseButtonsStates(window);
    mouseButtonsStates.state[button] = action; // for consistency (there is a tiny chance the state meanwhile changed)
    renderer->MouseCallback(int(x), int(y), button, action, mods, mouseButtonsStates.state);
}

void Display::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    renderer->ScrollCallback(int(x), int(y), int(xoffset), int(yoffset), GetMouseButtonsStates(window).state);
}

void Display::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

    renderer->CursorPosCallback(int(xpos), int(ypos), GetMouseButtonsStates(window).state);
}

void Display::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    renderer->KeyCallback(int(x), int(y), key, scancode, action, mods);
}

void Display::CharCallback(GLFWwindow* window, unsigned int codepoint)
{
    auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    renderer->CharCallback(int(x), int(y), codepoint);
}

void Display::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    glfwGetWindowSize(window, &width, &height);
    renderer->WindowSizeCallback(width, height);
}

void Display::LaunchRendering(bool loop) const
{
    const int num_extra_frames = 5;
    int frameCounter = 0;
    int windowWidth, windowHeight;

    // Main rendering loop
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    while (!glfwWindowShouldClose(window)) {
        double tic = igl::get_seconds();

        renderer->Draw();
        SwapBuffers();
        glfwPollEvents();

        if (frameCounter++ < num_extra_frames) {
            // in microseconds
            double duration = 1000000. * (igl::get_seconds() - tic);
            const double min_duration = 1000000. / 144.f;
            if (duration < min_duration) {
                std::this_thread::sleep_for(std::chrono::microseconds((int) (min_duration - duration)));
            }
        } else
            frameCounter = 0;

        if (!loop)
            break;

#ifdef __APPLE__
        static bool first_time_hack = true;
        if (first_time_hack) {
            glfwHideWindow(window);
            glfwShowWindow(window);
            first_time_hack = false;
        }
#endif
    }
}

void Display::SwapBuffers() const
{
    glfwSwapBuffers(window);
}

Display::~Display()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

static void ErrorCallback(int error, const char* description)
{
    fputs(description, stderr);
}

void APIENTRY DebugMessageCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes (note: id 7 are "will soon be deprecated" warnings)
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 7) return;

    std::cerr << "---------------" << std::endl;
    std::cerr << "Debug message (" << id << "): " << message << std::endl;

    switch (source) { // NOLINT(hicpp-multiway-paths-covered)
        case GL_DEBUG_SOURCE_API:
            std::cerr << "Source: API, ";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cerr << "Source: Window System, ";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cerr << "Source: Shader Compiler, ";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cerr << "Source: Third Party, ";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cerr << "Source: Application, ";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cerr << "Source: Other, ";
            break;
    }

    switch (type) { // NOLINT(hicpp-multiway-paths-covered)
        case GL_DEBUG_TYPE_ERROR:
            std::cerr << "Type: Error, ";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cerr << "Type: Deprecated Behaviour, ";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cerr << "Type: Undefined Behaviour, ";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cerr << "Type: Portability, ";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cerr << "Type: Performance, ";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cerr << "Type: Marker, ";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cerr << "Type: Push Group, ";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cerr << "Type: Pop Group, ";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cerr << "Type: Other, ";
            break;
    }

    switch (severity) { // NOLINT(hicpp-multiway-paths-covered)
        case GL_DEBUG_SEVERITY_HIGH:
            std::cerr << "Severity: High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cerr << "Severity: Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cerr << "Severity: Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cerr << "Severity: Notification";
            break;
    }

    std::cerr << std::endl;

#if !defined(NDEBUG) && defined(_MSC_VER)
    __debugbreak();
#endif
}

void Display::SetupDebugCallback()
{
    int contextFlags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        debug("Debug context created");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(DebugMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
}

GLFWwindow* Display::CreateWindow(const std::string& title, int width, int height, bool tryWithDebug)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_SAMPLES, 8);

    if (tryWithDebug) {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    } else {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

GLFWwindow* Display::SetupWindowAndContext(const std::string& title, int width, int height)
{
    glfwSetErrorCallback(ErrorCallback);

    auto window = CreateWindow(title, width, height, true);
    if (!window) {
        glfwTerminate();
        window = CreateWindow(title, width, height, false);
        if (!window) { // failed to create the window both with and without debug
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    }
    glfwMakeContextCurrent(window);

    // Load OpenGL and its extensions
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to load OpenGL and its extensions\n");
        exit(EXIT_FAILURE);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    return window;
}

} // namespace cg3d
