#include "Texture.h"
#include "gl.h"
#include "Utility.h"
#include <iostream>
#include <cassert>
#include <utility>

#include "stb/stb_image.h"


namespace cg3d
{

Texture::Texture(const std::string& file, int dim) : name(file), type(DimToType(dim))
{
    int width, height, numComponents;
    unsigned char* data;

    assert(dim > 0 && dim < 4);

    glGenTextures(1, &handle);
    glBindTexture(type, handle);
    switch (dim) {
        case 1:
            debug("loading ", dim, " dimensions texture file '", file, "'");
            data = LoadFromFile(file, &width, &height, &numComponents);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            break;

        case 2:
            debug("loading ", dim, " dimensions texture file '", file, "'");
            data = LoadFromFile(file, &width, &height, &numComponents);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            break;

        case 3: // cube map
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            {
                std::string directions[] = {"Right", "Left", "Top", "Bottom", "Front", "Back"};
                for (int i = 0; i < 6; i++) {
                    std::string fullFileName = file + directions[i] + ".bmp";
                    debug("loading cube map texture file '", fullFileName, "'");
                    data = LoadFromFile(fullFileName, &width, &height, &numComponents);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                }
            }
            break;
        default:
            throw std::range_error(std::string("invalid texture dimensions: ") + std::to_string(dim));
    }
    glBindTexture(type, 0);
    stbi_image_free(data);

    debug("created ", dim == 1 ? "1D" : dim == 3 ? "cube map" : "2D", " texture object ", handle, " of size ", width, "x", height, " pixels");
}

int Texture::DimToType(int dim)
{
    return dim == 1 ? GL_TEXTURE_1D : dim == 3 ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
}

Texture::Texture(std::string name, int width, int height, int dim, const void* data)
        : Texture(std::move(name), GL_RGBA, width, height, GL_RGBA, DimToType(dim), data) {}

Texture::Texture(std::string name, int internalformat, int width, int height, unsigned int format, unsigned int type, const void* data)
        : name(std::move(name)), type(type)
{
    assert(type == GL_TEXTURE_2D); // currently only 2D is supported

    glGenTextures(1, &handle);
    glBindTexture(type, handle);
    if (type == GL_TEXTURE_2D) { // todo: support textures other than 2D
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(type, 0);

    debug("created ", height > 0 ? "2D" : "1D", " texture object ", handle, " of size ", width, "x", height, " pixels");
}

Texture::~Texture()
{
    glDeleteTextures(1, &handle);
}

unsigned char* Texture::LoadFromFile(const std::string& fileName, int* width, int* height, int* numComponents)
{
    unsigned char* data = stbi_load(fileName.c_str(), width, height, numComponents, 4);
    if (data == nullptr) throw std::runtime_error(fileName + " stbi_load error");
    return data;
}

void Texture::Bind(int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(type, handle);
}

void Texture::Unbind(int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(type, 0);
}

} // namespace cg3d
