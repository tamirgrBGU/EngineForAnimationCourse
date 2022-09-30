#include "Test.h"
#include <iostream>

using namespace Eigen;
using namespace std;
using namespace igl::opengl;

#define PI_DIV_180 0.0174532f

void Test::Init()
{
    auto program = make_shared<Program>("shaders/basicShader", next_data_id++);

    auto bricks = make_shared<Material>(program);
    auto grass = make_shared<Material>(program);
    auto snake = make_shared<Material>(program);
    auto daylight = make_shared<Material>("shaders/cubemapShader", next_data_id++);

    bricks->AddTexture(0, "textures/bricks.jpg", 2);
    grass->AddTexture(0, "textures/grass.bmp", 2);
    snake->AddTexture(0, "textures/snake1.png", 2);
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);

    auto background = Shape::Create(Mesh::Cube(), daylight, shared_from_this()); // TODO: TAL: change parent to camera
	
    auto cube1 = Shape::Create(Mesh::Cube(), bricks, shared_from_this());
    auto cube2 = Shape::Create(Mesh::Cube(), bricks, shared_from_this());
    auto cylinder1 = Shape::Create(Mesh::Cylinder(), snake, shared_from_this());

    auto sphereMesh = make_shared<Mesh>("data/sphere.obj");
    auto sphere1 = Shape::Create(sphereMesh, grass, shared_from_this());
    auto sphere2 = Shape::Create(sphereMesh, grass, shared_from_this());

    background->Scale(60.0, Axis::All);
    background->SetStatic();

    cube1->SetShowWireframe();
    cube1->Translate(-3.5, Axis::X);
    cube2->Translate(3.5, Axis::X);

    sphere1->SetShowWireframe();
    sphere1->Scale(1.5, Axis::All);
    sphere1->Translate(3.0, Axis::Y);

    sphere2->Scale(1.5, Axis::All);
    sphere2->Translate(-3.0, Axis::Y);

    cylinder1->Scale(2, Axis::All);
    cylinder1->SetShowWireframe();

    // cube1->Rotate(0.7853982, Axis::Z);

     cylinder1->Rotate(90 * PI_DIV_180, Axis::Z);

    //cylinder1->Translate(3.5, Axis::X);

    //cylinder1->Rotate(1.570796, Axis::Z);
//    cylinder2->Translate(-2.0, Axis::X);
}

void Test::Update(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& model, const shared_ptr<const Program>& p)
{
    p->SetUniformMatrix4f("Proj", &proj);
    p->SetUniformMatrix4f("View", &view);
    p->SetUniformMatrix4f("Model", &model);
}
