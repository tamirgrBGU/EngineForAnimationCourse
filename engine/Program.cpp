#include "Program.h"
#include "Debug.h"
#include "gl.h"
#include "Shader.h"
#include <iostream>


namespace cg3d
{

Program::Program(const std::string &fileName, bool overlay, bool warnings) :
        Program(make_shared<Shader>(GL_VERTEX_SHADER, fileName + ".vs"),
                std::make_shared<Shader>(GL_FRAGMENT_SHADER, fileName + ".glsl"), overlay, warnings) {}

Program::Program(std::shared_ptr<const Shader> _vertexShader, std::shared_ptr<const Shader> _fragmentShader, bool overlay, bool warnings) :
        warnings(warnings), vertexShader(std::move(_vertexShader)), fragmentShader(std::move(_fragmentShader))
{
    handle = glCreateProgram();

    glAttachShader(handle, vertexShader->GetHandle());
    glAttachShader(handle, fragmentShader->GetHandle());

    if (overlay) {
        glBindAttribLocation(handle, (int) AttributesOverlay::OV_POSITION_VB, "position");
        glBindAttribLocation(handle, (int) AttributesOverlay::OV_COLOR, "color");
    } else {
        glBindAttribLocation(handle, (int) Attributes::POSITION_VB, "position");
        glBindAttribLocation(handle, (int) Attributes::NORMAL_VB, "normal");
        glBindAttribLocation(handle, (int) Attributes::KA_VB, "Ka");
        glBindAttribLocation(handle, (int) Attributes::KD_VB, "Kd");
        glBindAttribLocation(handle, (int) Attributes::KS_VB, "Ks");
        glBindAttribLocation(handle, (int) Attributes::TEXCOORD_VB, "texcoord");
    }

    glLinkProgram(handle);
    glValidateProgram(handle);

    debug("program object ", handle, " linked and validated");
}

Program::~Program()
{
    glDetachShader(handle, vertexShader->GetHandle());
    glDetachShader(handle, fragmentShader->GetHandle());
    glDeleteProgram(handle);
    debug("program object ", handle, " was deleted");
}

void Program::Bind() const
{
    glUseProgram(handle);
}

void Program::SetUniform1f(const std::string &name, float v0) const
{
    glUniform1f(GetUniformLocation(name), v0);
}

void Program::SetUniform2f(const std::string &name, float v0, float v1) const
{
    glUniform2f(GetUniformLocation(name), v0, v1);
}

void Program::SetUniform3f(const std::string &name, float v0, float v1, float v2) const
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Program::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3) const
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Program::SetUniform1i(const std::string &name, int v0) const
{
    glUniform1i(GetUniformLocation(name), v0);
}

void Program::SetUniform2i(const std::string &name, int v0, int v1) const
{
    glUniform2i(GetUniformLocation(name), v0, v1);
}

void Program::SetUniform3i(const std::string &name, int v0, int v1, int v2) const
{
    glUniform3i(GetUniformLocation(name), v0, v1, v2);
}

void Program::SetUniform4i(const std::string &name, int v0, int v1, int v2, int v3) const
{
    glUniform4i(GetUniformLocation(name), v0, v1, v2, v3);
}

void Program::SetUniform1ui(const std::string &name, unsigned int v0) const
{
    glUniform1ui(GetUniformLocation(name), v0);
}

void Program::SetUniform2ui(const std::string &name, unsigned int v0, unsigned int v1) const
{
    glUniform2ui(GetUniformLocation(name), v0, v1);
}

void Program::SetUniform3ui(const std::string &name, unsigned int v0, unsigned int v1, unsigned int v2) const
{
    glUniform3ui(GetUniformLocation(name), v0, v1, v2);
}

void
Program::SetUniform4ui(const std::string &name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) const
{
    glUniform4ui(GetUniformLocation(name), v0, v1, v2, v3);
}

void Program::SetUniform2fv(const std::string &name, int count, const Eigen::Vector2f *vectors) const
{
    glUniform2fv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniform3fv(const std::string &name, int count, const Eigen::Vector3f *vectors) const
{
    glUniform3fv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniform4fv(const std::string &name, int count, const Eigen::Vector4f *vectors) const
{
    glUniform4fv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniform2iv(const std::string &name, int count, const Eigen::Vector2i *vectors) const
{
    glUniform2iv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniform3iv(const std::string &name, int count, const Eigen::Vector3i *vectors) const
{
    glUniform3iv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniform4iv(const std::string &name, int count, const Eigen::Vector4i *vectors) const
{
    glUniform4iv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniformMatrix2f(const std::string &name, const Eigen::Matrix2f *matrix) const
{
    glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, matrix->data());
}

void Program::SetUniformMatrix3f(const std::string &name, const Eigen::Matrix3f *matrix) const
{
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, matrix->data());
}

void Program::SetUniformMatrix4f(const std::string &name, const Eigen::Matrix4f *matrix) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix->data());
}

void Program::SetUniformMatrix2fv(const std::string &name, int count, const Eigen::Matrix2f *matrices) const
{
    glUniformMatrix2fv(GetUniformLocation(name), count, GL_FALSE, matrices[0].data());
}

void Program::SetUniformMatrix3fv(const std::string &name, int count, const Eigen::Matrix3f *matrices) const
{
    glUniformMatrix3fv(GetUniformLocation(name), count, GL_FALSE, matrices[0].data());
}

void Program::SetUniformMatrix4fv(const std::string &name, int count, const Eigen::Matrix4f *matrices) const
{
    glUniformMatrix4fv(GetUniformLocation(name), count, GL_FALSE, matrices[0].data());
}

int Program::GetUniformLocation(const std::string &name) const
{
    if (uniformLocationCache.find(name) != uniformLocationCache.end())
        return uniformLocationCache[name];

    int location = glGetUniformLocation(handle, name.c_str());
    if (location == -1 && warnings)
        std::cerr << "Warning: ignoring uniform '" << name << "' (doesn't exist in program object " << handle << ")" << std::endl;

    uniformLocationCache[name] = location;
    return location;
}

std::shared_ptr<const Program> Program::GetFullWindowFixedColorQuadProgram()
{
    static auto PROGRAM = std::make_shared<const Program>(Shader::GetFullWindowQuadVertexShader(), Shader::GetFixedColorFragmentShader(), true, true);

    return PROGRAM;
}

} // namespace cg3d
