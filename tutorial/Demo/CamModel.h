#pragma once

#include "Camera.h"

#include <memory>
#include <utility>

class CamModel : public cg3d::Camera, public cg3d::Model
{
protected:
    CamModel(const Camera& camera, const Model& model) : Movable{model}, Camera{camera}, Model{model} {}; // NOLINT(cppcoreguidelines-slicing)

public:
    template<typename... Args>
    static std::shared_ptr<CamModel> Create(Args&&... args) { return std::shared_ptr<CamModel>(new CamModel{std::forward<Args>(args)...}); };

    ~CamModel() override = default;
    void Accept(cg3d::Visitor* visitor) override { Model::Accept(visitor); };
};
