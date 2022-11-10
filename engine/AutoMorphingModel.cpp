#include "AutoMorphingModel.h"

#include <utility>
#include "Utility.h"
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

} // namespace cg3d
