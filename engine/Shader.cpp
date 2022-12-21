#include <fstream>
#include <utility>
#include "Shader.h"
#include "Utility.h"
#include "gl.h"


namespace cg3d
{

std::shared_ptr<Shader> Shader::GetFixedColorFragmentShader()
{
    static auto SHADER = std::make_shared<Shader>(
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

std::shared_ptr<Shader> Shader::GetOverlayVertexShader()
{
    static auto SHADER = std::make_shared<Shader>(
            "Overlay vertex shader",
            GL_VERTEX_SHADER,
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

std::shared_ptr<Shader> Shader::GetFullWindowQuadVertexShader()
{
    static auto SHADER = std::make_shared<Shader>(
            "Full window quad vertex shader",
            GL_VERTEX_SHADER,
            R"(
#version 330
out vec2 texCoords;
void main()
{
    const vec2 vertices[3] = vec2[3](vec2(-1, -1), vec2(3, -1), vec2(-1, 3));
    gl_Position = vec4(vertices[gl_VertexID], 0, 1);
    texCoords = 0.5 * gl_Position.xy + vec2(0.5);
}
    		)");

    return SHADER;
}


std::shared_ptr<Shader> Shader::GetOverlayFragmentShader()
{
    static auto SHADER = std::make_shared<Shader>(
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

std::shared_ptr<Shader> Shader::GetBasicVertexShader()
{
    static auto SHADER = std::make_shared<Shader>(
            "Basic vertex shader",
            GL_FRAGMENT_SHADER,
            R"(
#version 330

attribute vec3 position;
attribute vec3 normal;
attribute vec4 Ka;
attribute vec4 Kd;
attribute vec4 Ks;
attribute vec2 texcoord;

out vec2 texCoord0;
out vec3 normal0;
out vec3 color0;
out vec3 position0;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

void main()
{
	texCoord0 = texcoord;
	color0 = vec3(Ka);
	normal0 = (Model  * vec4(normal, 0.0)).xyz;
	position0 = vec3(Proj * View * Model * vec4(position, 1.0));
	gl_Position = Proj * View * Model * vec4(position, 1.0); // you must have gl_Position
}
    		)");

    return SHADER;
}

std::shared_ptr<Shader> Shader::GetBasicFragmentShader()
{
    static auto SHADER = std::make_shared<Shader>(
            "Basic fragment shader",
            GL_FRAGMENT_SHADER,
            R"(
#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler2D sampler1;
uniform vec4 lightDirection;

out vec4 Color;

void main()
{
	Color = texture2D(sampler1, texCoord0)* vec4(color0,1.0);
}
    		)");

    return SHADER;
}

std::shared_ptr<Shader> Shader::GetOverlayPointsFragmentShader()
{
    static auto SHADER = std::make_shared<Shader>(
            "Overlay points fragment shader",
            GL_FRAGMENT_SHADER,
            R"(
#version 330
in vec3 color_frag;
out vec4 outColor;
void main()
{
 //   if (length(gl_PointCoord - vec2(0.5)) > 0.5)
 //       discard;
    outColor = vec4(color_frag, 1.0);
}
            )");

    return SHADER;
}

Shader::Shader(std::string _name, unsigned int type, const std::string& contents) : name(std::move(_name))
{
    handle = glCreateShader(type);
    const GLchar* p[1] = {contents.c_str()};
    int lengths[1] = {(int) contents.length()};
    glShaderSource(handle, 1, p, lengths);
    glCompileShader(handle);
    CheckCompileStatus(handle);
}

Shader::~Shader()
{
    glDeleteShader(handle);
    debug(name, "shader object ", handle, " was deleted");
}

void Shader::CheckCompileStatus(unsigned int shader)
{
    GLchar info[1024] = {0};
    glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
    debug("shader ", shader, " compile status: ", info[0] == 0 ? "no warnings or errors" : std::string("\n") + info);
}

std::string Shader::ReadFile(const std::string& fileName)
{
    debug("loading file ", fileName);
    std::ifstream file(fileName);
    if (!file.is_open())
        throw std::invalid_argument("error opening file " + fileName);

    file.seekg(0, std::ios::end);
    int len = (int) file.tellg();
    file.seekg(0);
    std::string contents(len + 1, '\0');
    file.read(&contents[0], len);
    return contents;
}

} // namespace cg3d
