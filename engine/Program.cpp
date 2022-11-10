#include "Program.h"
#include "Utility.h"
#include "gl.h"
#include "Shader.h"
#include <iostream>
#include <filesystem>
#include <utility>


namespace cg3d
{


namespace fs = std::filesystem;

Program::Program(const std::string &file, bool overlay, bool warnings) :
        Program(fs::path(file).filename().stem().generic_string(), make_shared<Shader>(GL_VERTEX_SHADER, file + ".vs"),
                std::make_shared<Shader>(GL_FRAGMENT_SHADER, file + ".glsl"), overlay, warnings) {}

Program::Program(std::string name, std::shared_ptr<Shader> _vertexShader, std::shared_ptr<Shader> _fragmentShader, bool overlay, bool warnings) :
        name(std::move(name)), warnings(warnings), vertexShader(std::move(_vertexShader)), fragmentShader(std::move(_fragmentShader))
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

    debug("program ", name, "(handle: ", handle, ") linked and validated");
}

Program::~Program()
{
    glDetachShader(handle, vertexShader->GetHandle());
    glDetachShader(handle, fragmentShader->GetHandle());
    glDeleteProgram(handle);
    debug("program ", name, "(handle: ", handle, ") was deleted");
}

void Program::Bind() const
{
    glUseProgram(handle);
}

void Program::SetUniform1f(const std::string &_name, float v0) const
{
    glUniform1f(GetUniformLocation(_name), v0);
}

void Program::SetUniform2f(const std::string &_name, float v0, float v1) const
{
    glUniform2f(GetUniformLocation(_name), v0, v1);
}

void Program::SetUniform3f(const std::string &_name, float v0, float v1, float v2) const
{
    glUniform3f(GetUniformLocation(_name), v0, v1, v2);
}

void Program::SetUniform4f(const std::string &_name, float v0, float v1, float v2, float v3) const
{
    glUniform4f(GetUniformLocation(_name), v0, v1, v2, v3);
}

void Program::SetUniform1i(const std::string &_name, int v0) const
{
    glUniform1i(GetUniformLocation(_name), v0);
}

void Program::SetUniform2i(const std::string &_name, int v0, int v1) const
{
    glUniform2i(GetUniformLocation(_name), v0, v1);
}

void Program::SetUniform3i(const std::string &_name, int v0, int v1, int v2) const
{
    glUniform3i(GetUniformLocation(_name), v0, v1, v2);
}

void Program::SetUniform4i(const std::string &_name, int v0, int v1, int v2, int v3) const
{
    glUniform4i(GetUniformLocation(_name), v0, v1, v2, v3);
}

void Program::SetUniform1ui(const std::string &_name, unsigned int v0) const
{
    glUniform1ui(GetUniformLocation(_name), v0);
}

void Program::SetUniform2ui(const std::string &_name, unsigned int v0, unsigned int v1) const
{
    glUniform2ui(GetUniformLocation(_name), v0, v1);
}

void Program::SetUniform3ui(const std::string &_name, unsigned int v0, unsigned int v1, unsigned int v2) const
{
    glUniform3ui(GetUniformLocation(_name), v0, v1, v2);
}

void
Program::SetUniform4ui(const std::string &_name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) const
{
    glUniform4ui(GetUniformLocation(_name), v0, v1, v2, v3);
}

void Program::SetUniform2fv(const std::string &_name, int count, const Eigen::Vector2f *vectors) const
{
    glUniform2fv(GetUniformLocation(_name), count, vectors[0].data());
}

void Program::SetUniform3fv(const std::string &_name, int count, const Eigen::Vector3f *vectors) const
{
    glUniform3fv(GetUniformLocation(_name), count, vectors[0].data());
}

void Program::SetUniform4fv(const std::string &_name, int count, const Eigen::Vector4f *vectors) const
{
    glUniform4fv(GetUniformLocation(_name), count, vectors[0].data());
}

void Program::SetUniform2iv(const std::string &_name, int count, const Eigen::Vector2i *vectors) const
{
    glUniform2iv(GetUniformLocation(_name), count, vectors[0].data());
}

void Program::SetUniform3iv(const std::string &_name, int count, const Eigen::Vector3i *vectors) const
{
    glUniform3iv(GetUniformLocation(_name), count, vectors[0].data());
}

void Program::SetUniform4iv(const std::string &_name, int count, const Eigen::Vector4i *vectors) const
{
    glUniform4iv(GetUniformLocation(_name), count, vectors[0].data());
}

void Program::SetUniformMatrix2f(const std::string &_name, const Eigen::Matrix2f *matrix) const
{
    glUniformMatrix2fv(GetUniformLocation(_name), 1, GL_FALSE, matrix->data());
}

void Program::SetUniformMatrix3f(const std::string &_name, const Eigen::Matrix3f *matrix) const
{
    glUniformMatrix3fv(GetUniformLocation(_name), 1, GL_FALSE, matrix->data());
}

void Program::SetUniformMatrix4f(const std::string &_name, const Eigen::Matrix4f *matrix) const
{
    glUniformMatrix4fv(GetUniformLocation(_name), 1, GL_FALSE, matrix->data());
}

void Program::SetUniformMatrix2fv(const std::string &_name, int count, const Eigen::Matrix2f *matrices) const
{
    glUniformMatrix2fv(GetUniformLocation(_name), count, GL_FALSE, matrices[0].data());
}

void Program::SetUniformMatrix3fv(const std::string &_name, int count, const Eigen::Matrix3f *matrices) const
{
    glUniformMatrix3fv(GetUniformLocation(_name), count, GL_FALSE, matrices[0].data());
}

void Program::SetUniformMatrix4fv(const std::string &_name, int count, const Eigen::Matrix4f *matrices) const
{
    glUniformMatrix4fv(GetUniformLocation(_name), count, GL_FALSE, matrices[0].data());
}

int Program::GetUniformLocation(const std::string &_name) const
{
    if (uniformLocationCache.find(_name) != uniformLocationCache.end())
        return uniformLocationCache[_name];

    int location = glGetUniformLocation(handle, _name.c_str());
    if (location == -1 && warnings)
        std::cerr << "Warning: ignoring uniform '" << _name << "' (doesn't exist in program object " << handle << ")" << std::endl;

    uniformLocationCache[_name] = location;
    return location;
}

std::shared_ptr<Program> Program::GetFullWindowFixedColorQuadProgram()
{
    static auto PROGRAM = std::make_shared<Program>("GetFullWindowFixedColorQuadProgram", Shader::GetFullWindowQuadVertexShader(),
                                                          Shader::GetFixedColorFragmentShader(), true, true);

    return PROGRAM;
}

} // namespace cg3d
