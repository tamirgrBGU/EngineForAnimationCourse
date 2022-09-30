#include "Material.h"

using std::make_shared;
using std::move;

Material::Material(shared_ptr<const Program> _program, bool overlay) :
    program(std::move(_program)),
    fixedColorProgram(make_shared<const Program>(std::move(program->GetVertexShader()), std::move(Shader::GetFixedColorFragmentShader()), overlay, false))
{}

Material::Material(const string& shaderFileName, bool overlay) : Material(make_shared<Program>(shaderFileName, overlay, true)) {}

void Material::AddTexture(int slot, shared_ptr<Texture> texture)
{
    textures.push_back(std::move(texture));
    textureSlots.push_back(slot);
}

void Material::AddTexture(int slot, const string& textureFileName, int dim)
{
    AddTexture(slot, make_shared<Texture>(textureFileName, dim));
}

shared_ptr<const Program> Material::BindProgram() const
{
    program->Bind();
    BindTextures();
    return program;
}

shared_ptr<const Program> Material::BindFixedColorProgram() const
{
    fixedColorProgram->Bind();
    return fixedColorProgram;
}

void Material::BindTextures() const
{
    for (int i = 0; i < textures.size(); i++) {
        textures[i]->Bind(textureSlots[i]);
        program->SetUniform1i("sampler" + to_string(i + 1), textureSlots[i]);
    }
}
