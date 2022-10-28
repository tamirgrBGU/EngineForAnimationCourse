#include "BasicScene.h"

using namespace cg3d;

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    SetNamedObject(camera, std::make_shared<Camera>, fov, float(width) / height, near, far);
   NewNamedObject(root, Movable::Create, shared_from_this());

    NewNamedObject(daylight, std::make_shared<Material>, "shaders/cubemapShader");
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    NewNamedObject(background, Model::Create, Mesh::Cube(), daylight, root);
    background->Scale(120, Axis::All);
    background->SetPickable(false);
    background->SetStatic();

 
    auto program = std::make_shared<Program>("shaders/basicShader");
    NewNamedObject(material, std::make_shared<Material>, program); // empty material
    SetNamedObject(cube, Model::Create, Mesh::Cube(), material, shared_from_this());
    material->AddTexture(0, "textures/carbon.jpg", 2);
    camera->Translate(15, Axis::Z);
    cube->Scale(3);
    auto mesh = cube->GetMeshList();
    std::cout<< "vertices: \n" << mesh[0]->data[0].vertices<<std::endl;
}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);

    cube->Rotate(0.01f, Axis::All);
}
