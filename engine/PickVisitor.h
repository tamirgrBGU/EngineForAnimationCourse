#include "Visitor.h"
#include <vector>


namespace cg3d
{

class PickVisitor : public Visitor
{
public:
    void Run(Scene* scene, Camera* camera) override;
    void Visit(Model* model) override;

    std::pair<Model*, float> PickAtPos(int x, int y);

private:
    std::vector<Model*> models;
    Scene* scene;
};

} // namespace cg3d
