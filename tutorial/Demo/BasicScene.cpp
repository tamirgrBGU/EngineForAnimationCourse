#include "BasicScene.h"

using namespace cg3d;

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    SetNamedObject(camera, std::make_shared<Camera>, fov, float(width) / height, near, far);
    auto program = std::make_shared<Program>("shaders/basicShader");
    NewNamedObject(material, std::make_shared<Material>, program); // empty material
    SetNamedObject(cube, Model::Create, Mesh::Cube(), material, shared_from_this());

    camera->Translate(15, Axis::Z);
    cube->Scale(3);
}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);

    cube->Rotate(0.01f, Axis::All);
}
