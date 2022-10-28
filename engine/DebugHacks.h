#include <string>
#include <GLFW/glfw3.h>
#include <memory>
#include "Eigen/Core"


namespace cg3d
{

class DebugHacks
{
    explicit DebugHacks(GLFWwindow* window) : window(window) {}

    static std::unique_ptr<DebugHacks> singleton;
    GLFWwindow* window;

public:

    static void Init(GLFWwindow* window);
    static DebugHacks* GetInstance();

    void DrawStencilBuffer(GLenum func, GLint ref, GLuint mask, const Eigen::Vector4f& color);

    DebugHacks(DebugHacks& other) = delete;
    void operator=(const DebugHacks&) = delete;
};

} // namespace cg3d
