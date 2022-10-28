#include "Visitor.h"
#include "Camera.h"

#include <utility>
#include <memory>


namespace cg3d
{

class DrawVisitor : public Visitor
{
public:
    explicit DrawVisitor(Scene* scene) : Visitor(scene) {}
    void Visit(Model* model) override;
    void Visit(Scene* scene) override;
    void Init() override;
    bool drawOutline = true;
    float outlineLineWidth = 5;

    Eigen::Vector4f outlineLineColor{1, 1, 1, 1};

private:
    void DrawOutline();
};

} // namespace cg3d
