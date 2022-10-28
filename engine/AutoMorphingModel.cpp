#include "AutoMorphingModel.h"

#include <utility>
#include "Debug.h"
#include "Visitor.h"


namespace cg3d
{

AutoMorphingModel::AutoMorphingModel(const Model& model, std::function<int(Model*, Visitor*)> calcMeshIndexFunc)
        : Movable{model.name}, Model{model}, CalcMeshIndexFunc{std::move(calcMeshIndexFunc)} {}

void AutoMorphingModel::Accept(Visitor* visitor)
{
    meshIndex = CalcMeshIndexFunc(this, visitor);

    Model::Accept(visitor);
}

std::shared_ptr<AutoMorphingModel> AutoMorphingModel::Create(const Model& model, std::function<int(Model*, Visitor*)> calcMeshIndexFunc, Movable* parent)
{
    auto autoMorphingModel = std::shared_ptr<AutoMorphingModel>(new AutoMorphingModel(model, std::move(calcMeshIndexFunc)));
    autoMorphingModel->SetParent(parent ? parent->shared_from_this() : model.parent.lock());
    return autoMorphingModel;
}

} // namespace cg3d
