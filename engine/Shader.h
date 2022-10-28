#pragma once

#include <string>
#include <unordered_map>
#include <Eigen/Core>


namespace cg3d
{

class Shader
{
public:

    std::string name;

    /**
        @brief Create shader object from the GLSL code in the given std::string
        @param name     - the name of the shader object
        @param contents - a std::string containing the code of the shader
        @param type     - the shader type
    **/
    Shader(std::string name, unsigned int type, const std::string& contents);

    /**
        @brief Create shader object from the GLSL code in the given file
        @param type     - the shader type
        @param file     - name of shader file to read
    **/
    Shader(unsigned int type, const std::string& file) : Shader(file, type, ReadFile(file)) {}

    static std::shared_ptr<const Shader> GetFixedColorFragmentShader();
    static std::shared_ptr<const Shader> GetBasicVertexShader();
    static std::shared_ptr<const Shader> GetBasicFragmentShader();
    static std::shared_ptr<const Shader> GetOverlayVertexShader();
    static std::shared_ptr<const Shader> GetOverlayFragmentShader();
    static std::shared_ptr<const Shader> GetOverlayPointsFragmentShader();
    static std::shared_ptr<const Shader> GetFullWindowQuadVertexShader();

    [[nodiscard]] inline unsigned int GetHandle() const { return handle; };

    ~Shader();

    // disable copy constructor and assignment operators
    void operator=(const Shader &shader) = delete;
    Shader(const Shader &shader) = delete;

private:
    unsigned int handle;
    static void CheckCompileStatus(unsigned int shader);
    static std::string ReadFile(const std::string& fileName);
};

} // namespace cg3d
