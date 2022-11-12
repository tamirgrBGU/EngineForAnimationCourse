#include "SimpleScene.h"
#include "Model.h"
#include "Renderer.h"
#include "ObjLoader.h"
#include "CamModel.h"


using namespace cg3d;

void SimpleScene::Init(float fov, int width, int height, float near, float far)
{
    camera = Camera::Create("camera", fov, float(width) / float(height), near, far);
    camera->Translate(15, Axis::Z);

    // create the root object that will contain all visible models
    auto root = Movable::Create("root");
    AddChild(root);

    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")};
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    root->AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();

    auto program = std::make_shared<Program>("shaders/basicShader"); // todo: replace with hard-coded basic program
    auto material{std::make_shared<Material>("material", program)}; // default material
    auto cube1{Model::Create("cube1", Mesh::Cube(), material)};
    auto cube2{Model::Create("cube2", Mesh::Cube(), material)};
    auto cube3{Model::Create("cube3", Mesh::Cube(), material)};
    auto cube4{Model::Create("cube4", Mesh::Cube(), material)};

    AddChildren({cube1, cube2, cube3, cube4});

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
    auto generatedTexture{std::make_shared<Texture>("generatedTexture", width, height, 2, data)};
    delete[] data;
    return generatedTexture;
}
