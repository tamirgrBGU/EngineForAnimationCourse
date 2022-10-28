#include "Visitor.h"
#include <vector>


namespace cg3d
{

class PickVisitor : public Visitor
{
public:
    explicit PickVisitor(Scene* scene) : Visitor(scene) {}

    void Run(Camera* camera) override;
    void Init() override;
    void Visit(Model* model) override;

    std::pair<Model*, float> PickAtPos(int x, int y);

private:
    std::vector<Model*> models;
};

} // namespace cg3d
