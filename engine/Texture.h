#pragma once

#include <string>

using std::string;

class Texture
{
public:

    string name;

    /**
        @brief Create a texture object from an existing image file
        @param file - the name of the image file
        @param dim      - the dimensions of the texture data
    **/
    Texture(const string &file, int dim);

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
    Texture(string name, int internalformat, int width, int height, unsigned int format, unsigned int type, const void* data = nullptr);

    void Bind(int slot) const;

    void Unbind(int slot) const;

    inline unsigned int GetHandle() const { return handle; }

    ~Texture();

private:

    static unsigned char* LoadFromFile(const string& fileName, int* width, int* height, int* numComponents);

    unsigned int handle = 0;
    unsigned int type = 0;
};
