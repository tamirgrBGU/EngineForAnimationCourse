#pragma once

#include "Shader.h"
#include <string>


namespace cg3d
{

class Program
{
public:
    std::string name;

    /**
        @brief Create a program object with a vertex and fragment shader from 2 files
        @param file - name of the files without extension (extension .vs and .glsl is appended)
        @param overlay  - true for full overlay data, false for partial data
    **/
    explicit Program(const std::string &file, bool overlay = false, bool warnings = true);

/**
	 @brief Create a program object from the given vertex and fragment shader objects
	 @param name            - the program name (for debug purposes)
	 @param _vertexShader	- a vertex shader object
	 @param _fragmentShader	- a fragment shader object
	 @param overlay			- true for full overlay data, false for partial data
 **/
    Program(std::string name, std::shared_ptr<Shader> _vertexShader, std::shared_ptr<Shader> _fragmentShader, bool overlay, bool warnings);

    /**
        @brief BindColorBuffer the program object
    **/
    void Bind() const;

    inline unsigned int GetHandle() const { return handle; }
    inline std::shared_ptr<Shader> GetVertexShader() const { return vertexShader; }
    ~Program();

///@{
    /**
       Mapping methods between Eigen and OpenGL shader language uniforms
       See @ref https://docs.gl/gl4/glUniform
    **/
    void SetUniform1f(const std::string &name, float v0) const;
    void SetUniform2f(const std::string &name, float v0, float v1) const;
    void SetUniform3f(const std::string &name, float v0, float v1, float v2) const;
    void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3) const;
    void SetUniform1i(const std::string &name, int v0) const;
    void SetUniform2i(const std::string &name, int v0, int v1) const;
    void SetUniform3i(const std::string &name, int v0, int v1, int v2) const;
    void SetUniform4i(const std::string &name, int v0, int v1, int v2, int v3) const;
    void SetUniform1ui(const std::string &name, unsigned int v0) const;
    void SetUniform2ui(const std::string &name, unsigned int v0, unsigned int v1) const;
    void SetUniform3ui(const std::string &name, unsigned int v0, unsigned int v1, unsigned int v2) const;
    void SetUniform4ui(const std::string &name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) const;
    void SetUniform2fv(const std::string &name, int count, const Eigen::Vector2f *vectors) const;
    void SetUniform3fv(const std::string &name, int count, const Eigen::Vector3f *vectors) const;
    void SetUniform4fv(const std::string &name, int count, const Eigen::Vector4f *vectors) const;
    void SetUniform2iv(const std::string &name, int count, const Eigen::Vector2i *vectors) const;
    void SetUniform3iv(const std::string &name, int count, const Eigen::Vector3i *vectors) const;
    void SetUniform4iv(const std::string &name, int count, const Eigen::Vector4i *vectors) const;
    void SetUniformMatrix2f(const std::string &name, const Eigen::Matrix2f *matrix) const;
    void SetUniformMatrix3f(const std::string &name, const Eigen::Matrix3f *matrix) const;
    void SetUniformMatrix4f(const std::string &name, const Eigen::Matrix4f *matrix) const;
    void SetUniformMatrix2fv(const std::string &name, int count, const Eigen::Matrix2f *matrices) const;
    void SetUniformMatrix3fv(const std::string &name, int count, const Eigen::Matrix3f *matrices) const;
    void SetUniformMatrix4fv(const std::string &name, int count, const Eigen::Matrix4f *matrices) const;
///@}

    static std::shared_ptr<Program> GetFullWindowFixedColorQuadProgram();

    // disable copy constructor and assignment operator
    void operator=(const Program &shader) = delete;

    Program(const Program &shader) = delete;

private:

    int GetUniformLocation(const std::string &name) const;

    std::shared_ptr<Shader> vertexShader;
    std::shared_ptr<Shader> fragmentShader;
    unsigned int handle;
    mutable std::unordered_map<std::string, int> uniformLocationCache;
    bool warnings;

    enum class Attributes
    {
        POSITION_VB,
        NORMAL_VB,
        KA_VB,
        KD_VB,
        KS_VB,
        TEXCOORD_VB,
        JOINT_INDEX_VB
    };

    enum class AttributesOverlay
    {
        OV_POSITION_VB,
        OV_COLOR,
    };
};

} // namespace cg3d
