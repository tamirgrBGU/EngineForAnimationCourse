#include "glad/glad.h"
#include "DebugHacks.h"
#include "Program.h"
#include <stdexcept>


namespace cg3d
{

std::unique_ptr<DebugHacks> DebugHacks::singleton = nullptr;

void DebugHacks::Init(GLFWwindow* window)
{
    singleton = std::unique_ptr<DebugHacks>(new DebugHacks(window));
}

DebugHacks* DebugHacks::GetInstance()
{
    if (!singleton)
        throw std::runtime_error("DebugHacks must be initialized to be used");

    return singleton.get();
}

void DebugHacks::DrawStencilBuffer(GLenum func, GLint ref, GLuint mask, const Eigen::Vector4f& color)
{
    auto program = Program::GetFullWindowFixedColorQuadProgram();
    program->Bind();
    program->SetUniform4fv("fixedColor", 1, &color);

    glStencilFunc(func, ref, mask);

    GLuint vao;
    glEnable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilMask(0x0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);

    glDeleteVertexArrays(1, &vao);
}

} // namespace cg3d
