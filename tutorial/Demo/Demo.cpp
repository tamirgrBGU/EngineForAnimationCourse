#include "Renderer.h"
#include "Display.h"

#define SCENE SceneWithCameras // the scene (class name) to display

#define STRINGIFY(X) #X
#define CLASS_NAME_TO_HEADER(X) STRINGIFY(X.h)
#include CLASS_NAME_TO_HEADER(SCENE)


using namespace cg3d;

int main()
{
    const int DISPLAY_WIDTH = 800;
    const int DISPLAY_HEIGHT = 800;
    const float CAMERA_ANGLE = 45.0f;
    const float NEAR = 0.1f;
    const float FAR = 120.0f;

    Renderer renderer;
    Display display("Demo", DISPLAY_WIDTH, DISPLAY_HEIGHT, &renderer);
    std::cout.setstate(std::ios_base::failbit); // suppress junk output to console from igl::opengl::glfw::Viewer
    NewNamedObject(scene, std::make_shared<SCENE>, &display);
    std::cout.clear(); // re-enable output to console
    auto viewport = std::make_shared<Viewport>(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    scene->Init(CAMERA_ANGLE, DISPLAY_WIDTH, DISPLAY_HEIGHT, NEAR, FAR);
    renderer.AddViewport(scene);
    display.LaunchRendering(true);

    return 0;
}
