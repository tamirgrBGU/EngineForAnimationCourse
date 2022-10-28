#pragma once

#include "Scene.h"
#include "Texture.h"
#include <memory>
#include <utility>

class SimpleScene : public cg3d::Scene
{
public:
    explicit SimpleScene(std::string name, cg3d::Display* display) : Scene(std::move(name), display) {};
    void Init(float fov, int width, int height, float near, float far);

private:
    static std::shared_ptr<cg3d::Texture> GenerateTexture(int width, int height);
};
