#pragma once

#include <vector>
#include "Texture.h"
#include "Program.h"

using namespace std;

class Material
{
    vector<shared_ptr<Texture>> textures;
    vector<int> textureSlots;

public:

    const shared_ptr<const Program> program, fixedColorProgram;

    /**
        @brief Create a material with a given program object
        @param program - shared pointer to the program object
    **/
    explicit Material(shared_ptr<const Program> program, bool overlay = false);

    /**
        @brief Create a program object from the shader files and use it to create a material object
        (use for convenience when the shader objects are exclusive to this material)
        @param shaderFileName - filename (without extension) of the shaders
        @param overlay        - overlay of the program object
        @param programId      - id of the program object
    **/
    explicit Material(const string& shaderFileName, bool overlay = false);

    /**
        @brief Add a given texture object to the material
        @param slot    - the slot to bind the texture to when binding the material
        @param texture - shared pointer to the texture object
    **/
    void AddTexture(int slot, shared_ptr<Texture> texture);

    /**
        @brief Creates a texture object from an existing image file and add it to the material
        (use for convenience when the texture object is exclusive to this material)
        @param slot            - the slot to bind the texture to when binding the material
        @param textureFileName - the name of the image file
        @param dim             - the dimensions of the texture data
    **/
    void AddTexture(int slot, const string &textureFileName, int dim);

    /**
        @brief Binds the main material program
        @retval  - a shared pointer to the program object
    **/
    shared_ptr<const Program> BindProgram() const;

    /**
        @brief Binds the picking material program
        @retval  - a shared pointer to the picking program object
    **/
    shared_ptr<const Program> BindFixedColorProgram() const;

    /**
        @brief Binds all associated textures
    **/
    void BindTextures() const;
};
