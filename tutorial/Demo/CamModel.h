#pragma once

#include "Camera.h"

#include <memory>
#include <utility>

class CamModel : public cg3d::Camera, public cg3d::Model
{
public:

    CamModel(const Camera& camera, const Model& model) : Movable{model}, Camera{camera}, Model{model} {}; // NOLINT(cppcoreguidelines-slicing)
    ~CamModel() override = default;

    void Accept(cg3d::Visitor* visitor) override { Model::Accept(visitor); };
};
