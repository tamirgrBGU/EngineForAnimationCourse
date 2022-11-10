#include "Model.h"


namespace cg3d
{

class AutoMorphingModel : public Model
{
protected:
    AutoMorphingModel(const Model& model, std::function<int(Model*, Visitor*)> calcMeshIndexFunc);

public:
    template<typename... Args>
    static std::shared_ptr<AutoMorphingModel> Create(Args&&... args) { return std::make_shared<AutoMorphingModel>(AutoMorphingModel{std::forward<Args>(args)...}); };
    void Accept(Visitor* visitor) override;
    std::function<int(Model*, Visitor*)> CalcMeshIndexFunc;
};

} // namespace cg3d
