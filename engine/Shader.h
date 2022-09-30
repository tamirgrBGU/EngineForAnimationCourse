#pragma once

#include <string>
#include <unordered_map>
#include <Eigen/Core>

using namespace std;

class Shader
{
public:

    const string name;

    /**
        @brief Create shader object from the GLSL code in the given string
        @param name     - the name of the shader object
        @param contents - a string containing the code of the shader
        @param type     - the shader type
    **/
    Shader(const string& name, unsigned int type, const string& contents);

    /**
        @brief Create shader object from the GLSL code in the given file
        @param type     - the shader type
        @param file     - name of shader file to read
    **/
    Shader(unsigned int type, const string& file) : Shader(file, type, ReadFile(file)) {}

    static shared_ptr<const Shader> GetFixedColorFragmentShader();

    static shared_ptr<const Shader> GetOverlayVertexShader();

    static shared_ptr<const Shader> GetOverlayFragmentShader();

    static shared_ptr<const Shader> GetOverlayPointsFragmentShader();

    unsigned int GetHandle() const;

    ~Shader();

    // disable copy constructor and assignment operators
    void operator=(const Shader &shader) = delete;
    Shader(const Shader &shader) = delete;

private:

    unsigned int handle;

    static void CheckCompileStatus(const string& name, unsigned int shader);

    static string ReadFile(const string& fileName);
};
