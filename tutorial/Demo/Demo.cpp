#include "Demo.h"
#include "Shape.h"
#include <iostream>

using namespace Eigen;
using namespace std;
using namespace igl::opengl;

#define PI_DIV_180 0.0174532f

void Demo::Init()
{
    camera1->Translate(20, Axis::Z);

    camera2->Translate(15, Axis::Z);
    camera2->Translate(-10, Axis::X);

    camera3->Translate(15, Axis::Z);
    camera3->Translate(10, Axis::X);

    auto program = make_shared<Program>("shaders/basicShader");

    auto bricks = make_shared<Material>(program);
    auto grass = make_shared<Material>(program);
    auto snake = make_shared<Material>(program);
    auto daylight = make_shared<Material>("shaders/cubemapShader");

    bricks->AddTexture(0, "textures/bricks.jpg", 2);
    grass->AddTexture(0, "textures/grass.bmp", 2);
    snake->AddTexture(0, "textures/snake1.png", 2);
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);

    NewNamedObject(parent, Movable::Create, shared_from_this()); // we can use a parent container for some or all of the shapes
    NewNamedObject(background, Shape::Create, Mesh::Cube(), daylight, parent);
    NewNamedObject(cube1, Shape::Create, Mesh::Cube(), bricks, parent);
    NewNamedObject(cube2, Shape::Create, Mesh::Cube(), bricks, parent);
    NewNamedObject(cylinder1, Shape::Create, Mesh::Cylinder(), snake, parent);

    auto sphereMesh = make_shared<Mesh>("data/sphere.obj");
    NewNamedObject(sphere1, Shape::Create, sphereMesh, grass, parent);
    NewNamedObject(sphere2, Shape::Create, sphereMesh, grass, parent);

    background->Scale(60.0, Axis::All);
    background->SetPickable(false);
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

    cylinder1->Rotate(90 * PI_DIV_180, Axis::Z);
}

void Demo::Update(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& model, const shared_ptr<const Program>& p)
{
    p->SetUniformMatrix4f("Proj", &proj);
    p->SetUniformMatrix4f("View", &view);
    p->SetUniformMatrix4f("Model", &model);

    if (isActive) {
        currentCamera->Translate(-0.001f, Axis::Z);
        currentCamera->Rotate(0.0001f, Axis::Z);
    }
}
