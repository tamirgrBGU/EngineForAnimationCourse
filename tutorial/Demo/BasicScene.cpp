#include "BasicScene.h"

using namespace cg3d;

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    camera = Camera::Create("camera", fov, float(width) / float(height), near, far);
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material = std::make_shared<Material>("material", program); // empty material
    cube = Model::Create("cube", Mesh::Cube(), material);
    AddChild(cube);

    camera->Translate(15, Axis::Z);
    cube->Scale(3);
}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);

    cube->Rotate(0.01f, Axis::XYZ);
}
