#include <fstream>
#include "Shader.h"
#include "Debug.h"
#include "gl.h"

shared_ptr<const Shader> Shader::GetFixedColorFragmentShader()
{
    static auto SHADER = make_shared<const Shader>(
            "Fixed color fragment shader",
            GL_FRAGMENT_SHADER,
            R"(
#version 330
uniform vec4 fixedColor;
out vec4 Color;
void main()
{
    Color = fixedColor;
}
            )");

    return SHADER;
}

shared_ptr<const Shader> Shader::GetOverlayVertexShader()
{
    static auto SHADER = make_shared<const Shader>(
            "Overlay vertex shader",
            GL_FRAGMENT_SHADER,
            R"(
#version 330

in vec3 position;
in vec3 color;
out vec3 color_frag;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

void main()
{
    color_frag = color;
    gl_Position = Proj * View * Model * vec4 (position, 1.0);
}
    		)");

    return SHADER;
}

shared_ptr<const Shader> Shader::GetOverlayFragmentShader()
{
    static auto SHADER = make_shared<const Shader>(
            "Overlay fragment shader",
            GL_FRAGMENT_SHADER,
            R"(
#version 330
in vec3 color_frag;
out vec4 outColor;

void main()
{
    outColor = vec4(color_frag, 1.0);
}
    		)");

    return SHADER;
}

shared_ptr<const Shader> Shader::GetOverlayPointsFragmentShader()
{
    static auto SHADER = make_shared<const Shader>(
            "Overlay points fragment shader",
            GL_FRAGMENT_SHADER,
            R"(
#version 330
in vec3 color_frag;
out vec4 outColor;
void main()
{
    if (length(gl_PointCoord - vec2(0.5)) > 0.5)
        discard;
    outColor = vec4(color_frag, 1.0);
}
            )");

    return SHADER;
}

Shader::Shader(const string& name, unsigned int type, const string& contents)
{
    handle = glCreateShader(type);
    const GLchar *p[1] = {contents.c_str()};
    int lengths[1] = {(int) contents.length()};
    glShaderSource(handle, 1, p, lengths);
    glCompileShader(handle);
    CheckCompileStatus(name, handle);
}

unsigned int Shader::GetHandle() const
{
    return handle;
}

Shader::~Shader()
{
    glDeleteShader(handle);
    debug(name, "shader object ", handle, " was deleted");
}

void Shader::CheckCompileStatus(const string& name, unsigned int shader)
{
    GLchar info[1024] = {0};
    glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
    debug("shader ", shader, " compile status: ", info[0] == 0 ? "no warnings or errors" : string("\n") + info);
}

string Shader::ReadFile(const string &fileName)
{
    debug("loading file ", fileName);
    ifstream file(fileName);
    if (!file.is_open())
        throw invalid_argument("error opening file " + fileName);

    file.seekg(0, ios::end);
    int len = (int)file.tellg();
    file.seekg(0);
    string contents(len + 1, '\0');
    file.read(&contents[0], len);
    return contents;
}