#include "SimpleScene.h"
#include "Model.h"
#include "Renderer.h"
#include "ObjLoader.h"
#include "CamModel.h"


using namespace cg3d;

void SimpleScene::Init(float fov, int width, int height, float near, float far)
{
    SetNamedObject(camera, std::make_shared<Camera>, fov, float(width) / height, near, far);
    camera->Translate(15, Axis::Z);

    // create the root object that will contain all visible models
    NewNamedObject(root, Movable::Create, shared_from_this());

    NewNamedObject(daylight, std::make_shared<Material>, "shaders/cubemapShader");
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    NewNamedObject(background, Model::Create, Mesh::Cube(), daylight, root);
    background->Scale(120, Axis::All);
    background->SetPickable(false);
    background->SetStatic();

    auto program = std::make_shared<Program>("shaders/basicShader"); // TODO: TAL: replace with hard-coded basic program
    NewNamedObject(material, std::make_shared<Material>, program); // default material
    NewNamedObject(cube1, Model::Create, Mesh::Cube(), material, root);
    NewNamedObject(cube2, Model::Create, Mesh::Cube(), material, root);
    NewNamedObject(cube3, Model::Create, Mesh::Cube(), material, root);
    NewNamedObject(cube4, Model::Create, Mesh::Cube(), material, root);

    material->AddTexture(0, "textures/carbon.jpg", 2);
    cube1->Translate(-4.5, Axis::X);
    cube2->Translate(-1.5, Axis::X);
    cube3->Translate(1.5, Axis::X);
    cube4->Translate(4.5, Axis::X);

    cube2->RotateByDegree(45, Axis::X);
    cube3->RotateByDegree(45, Axis::Y);
    cube4->RotateByDegree(45, Axis::Z);


}

std::shared_ptr<Texture> SimpleScene::GenerateTexture(int width, int height)
{
    auto data = new unsigned char[width * height * 4];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            data[4 * (i * width + j)] = (i + j) % 256;
            data[4 * (i * width + j) + 1] = (i + 7 * (j + 1)) % 256;
            data[4 * (i * width + j) + 2] = (i + 5 * (j + 2)) % 256;
            data[4 * (i * width + j) + 3] = 255;
        }
    }
    NewNamedObject(generatedTexture, std::make_shared<Texture>, width, height, 2, data);
    delete[] data;
    return generatedTexture;
}
