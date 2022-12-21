#include "PickVisitor.h"

#include <GL.h>
#include "Model.h"
#include "Scene.h"
#include "Utility.h"


namespace cg3d
{

void PickVisitor::Run(Scene* _scene, Camera* camera)
{
    // clear everything
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(0, 0, 0, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);

    Visitor::Run(scene = _scene, camera);
}

std::pair<Model*, float> PickVisitor::PickAtPos(int x, int y)
{
    float depth;
    unsigned char data[]{0, 0, 0, 0};
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    int id = data[0] | data[1] << 8 | data[2] << 16;
    Model* model = id == 0 ? nullptr : models[id - 1];
    return {model, depth};
}

void PickVisitor::Visit(Model* model)
{
    Visitor::Visit(model); // draw children first

    // note: non-pickable models can still be picked in order to get the depth and such
    // the responsibility to *not* pick them up is on the caller's side

    if (!model->isHidden) {
        auto& program = *model->material->BindFixedColorProgram();
        scene->Update(program, proj, view, model->isStatic ? model->GetAggregatedTransform() : norm * model->GetAggregatedTransform());
        models.emplace_back(model);

        int id = int(models.size()); // temporary id for the model (translated to color below)
        int r = (id & 0x000000FF) >> 0;
        int g = (id & 0x000000FF) >> 8;
        int b = (id & 0x000000FF) >> 16;
        program.SetUniform4f("fixedColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f); // NOLINT(cppcoreguidelines-narrowing-conversions)
        model->UpdateDataAndDrawMeshes(program, true, false);
    }
}

} // namespace cg3d
