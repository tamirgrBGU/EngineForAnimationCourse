#include "Visitor.h"
#include "Scene.h"


namespace cg3d
{

void Visitor::Run(Scene* scene, Camera* camera)
{
    proj = camera->GetViewProjection();
    view = camera->GetAggregatedTransform().inverse();
    norm = scene->GetAggregatedTransform();

    scene->Accept(this);
}

void Visitor::Visit(Movable* movable)
{
    for (const auto& child: movable->children)
        child->Accept(this);
}

void Visitor::Visit(Scene* scene)
{
    Visit(static_cast<Movable*>(scene));
}

void Visitor::Visit(Model* model)
{
    Visit(static_cast<Movable*>(model));
}

} // namespace cg3d
