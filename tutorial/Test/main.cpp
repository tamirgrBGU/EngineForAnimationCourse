#pragma warning(push, 3)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)

#include "Renderer.h"
#include "Test.h"
#include "InputManager.h"
#include "Display.h"

#pragma warning(pop)

int main(int argc,char *argv[])
{
    const int DISPLAY_WIDTH = 800;
    const int DISPLAY_HEIGHT = 800;
    const float CAMERA_ANGLE = 45.0f;
    const float NEAR = 1.0f;
    const float FAR = 120.0f;
    std::list<int> x, y;
    x.push_back(DISPLAY_WIDTH);
    y.push_back(DISPLAY_HEIGHT);
    Display disp = Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OPENGL", true);
    //igl::opengl::glfw::imgui::ImGuiMenu* menu = new igl::opengl::glfw::imgui::ImGuiMenu();
    Renderer* rndr = new Renderer(CAMERA_ANGLE, (float)DISPLAY_WIDTH/(float)DISPLAY_HEIGHT, NEAR, FAR);
    auto scene = make_shared<Test>();

    Init(disp); //adding callback functions
    scene->Init();    //adding programs, textures, shapes to scene
    rndr->Init(scene.get(),x,y,1); // adding scene and viewports to the renderer
    //rndr->SetBuffers();
    disp.SetRenderer(rndr);

    disp.launch_rendering(rndr);

    delete rndr;

    return 0;
}

