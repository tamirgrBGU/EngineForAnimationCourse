#include "Model.h"


namespace cg3d
{

class AutoMorphingModel : public Model
{
    AutoMorphingModel(const Model& model, std::function<int(Model*, Visitor*)> calcMeshIndexFunc);

public:
    void Accept(Visitor* visitor) override;
    static std::shared_ptr<AutoMorphingModel> Create(const Model& model, std::function<int(Model*, Visitor*)> calcMeshIndexFunc, Movable* parent = nullptr);

    std::function<int(Model*, Visitor*)> CalcMeshIndexFunc;
};

} // namespace cg3d
