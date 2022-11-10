#pragma once

#include <string>


namespace cg3d
{

class Texture
{
public:
    std::string name;

    /**
        @brief Create a texture object from an existing image file
        @param file - the name of the image file
        @param dim  - the dimensions of the texture data
    **/
    Texture(const std::string &file, int dim);

    /**
        @brief Create a texture object from pixel data in memory.
        Parameter information at @ref https://docs.gl/gl4/glTexImage2D
        @param name           - name of the texture object (for debugging)
        @param internalformat - number of color components in the texture (see link)
        @param width          - width of the image
        @param height         - height of the image
        @param format         - format of the pixel data
        @param type           - data type of the pixel data
        @param data           - a pointer to the image data in memory (omit to create empty texture)
    **/
    Texture (std::string name, int internalformat, int width, int height, unsigned int format, unsigned int type, const void* data = nullptr);

    /**
        @brief Create a texture object from pixel data in memory in GL_RGBA format
        Parameter information at @ref https://docs.gl/gl4/glTexImage2D
        @param name           - name of the texture object (for debugging)
        @param width          - width of the image
        @param height         - height of the image
        @param dim            - the dimensions of the texture data (currently only 2D is supported)
        @param data           - a pointer to the image data in memory (omit to create empty texture)
    **/
    Texture(std::string name, int width, int height, int dim, const void* data);

    void Bind(int slot) const;
    void Unbind(int slot) const;
    ~Texture();

private:

    static unsigned char* LoadFromFile(const std::string& fileName, int* width, int* height, int* numComponents);

    unsigned int handle = 0;
    unsigned int type = 0;
    static int DimToType(int dim) ;
};

} // namespace cg3d
