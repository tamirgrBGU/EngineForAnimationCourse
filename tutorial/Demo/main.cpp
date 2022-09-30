#pragma warning(push, 3)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)

#include "Renderer.h"
#include "Demo.h"
#include "InputManager.h"
#include "Display.h"
#include <memory>

#pragma warning(pop)

using std::make_shared;

int main()
{
    const int DISPLAY_WIDTH = 800;
    const int DISPLAY_HEIGHT = 800;
    const float CAMERA_ANGLE = 45.0f;
    const float NEAR = 1.0f;
    const float FAR = 120.0f;

    std::list<int> x, y;
    x.push_back(DISPLAY_WIDTH);
    y.push_back(DISPLAY_HEIGHT);
    Display display = Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OPENGL", true);
    //igl::opengl::glfw::imgui::ImGuiMenu* menu = new igl::opengl::glfw::imgui::ImGuiMenu();
    auto renderer = new Renderer();

    NewNamedObject(scene, make_shared<Demo>);
    SetNamedObject(scene->camera1, Camera::Create, CAMERA_ANGLE, (float) DISPLAY_WIDTH / (float) DISPLAY_HEIGHT, NEAR, FAR, scene);
    SetNamedObject(scene->camera2, Camera::Create, CAMERA_ANGLE, (float) DISPLAY_WIDTH / (float) DISPLAY_HEIGHT, NEAR, FAR, scene);
    SetNamedObject(scene->camera3, Camera::Create, CAMERA_ANGLE, (float) DISPLAY_WIDTH / (float) DISPLAY_HEIGHT, NEAR, FAR, scene);
    scene->currentCamera = scene->camera1;

    Init(display); // add callback functions
    scene->Init(); // add programs, textures and shapes to scene
    renderer->Init(scene.get(), x, y, scene->camera1, 1); // adding scene and viewports to the renderer
    //renderer->SetBuffers();
    display.SetRenderer(renderer);
    display.LaunchRendering(renderer);

    delete renderer;

    return 0;
}

