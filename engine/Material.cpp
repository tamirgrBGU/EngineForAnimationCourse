#include "Material.h"

#include <utility>

namespace cg3d
{

Material::Material(std::string name, std::shared_ptr<Program> _program, bool overlay) : name(std::move(name)), program(std::move(_program)),
        fixedColorProgram(std::make_shared<Program>(program->name + "_fixedColorShader", program->GetVertexShader(), Shader::GetFixedColorFragmentShader(), overlay, false)) {}

Material::Material(std::string name, const std::string& fileWithoutExt, bool overlay) :
        Material(std::move(name), std::make_shared<Program>(fileWithoutExt, overlay, true)) {}

void Material::AddTexture(int slot, std::shared_ptr<Texture> texture)
{
    textures.push_back(std::move(texture));
    textureSlots.push_back(slot);
}

void Material::AddTexture(int slot, const std::string& textureFileName, int dim)
{
    AddTexture(slot, std::make_shared<Texture>(textureFileName, dim));
}

const Program* Material::BindProgram() const
{
    program->Bind();
    return program.get();
}

const Program* Material::BindFixedColorProgram() const
{
    fixedColorProgram->Bind();
    return fixedColorProgram.get();
}

void Material::BindTextures() const
{
    for (int i = 0; i < textures.size(); i++) {
        textures[i]->Bind(textureSlots[i]);
        program->SetUniform1i("sampler" + std::to_string(i + 1), textureSlots[i]);
    }
}

} // namespace cg3d
