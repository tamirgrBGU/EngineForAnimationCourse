#include "Visitor.h"
#include "Scene.h"


namespace cg3d
{

void Visitor::Run(Camera* camera)
{
    proj = camera->GetViewProjection();
    view = camera->GetAggregatedTransform().inverse();
    norm = scene->aggregatedTransform;

    Init();

    scene->Accept(this);
}

} // namespace cg3d
