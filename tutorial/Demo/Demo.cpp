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
    auto scene = std::make_shared<SCENE>(STRINGIFY(SCENE), &display);
    std::cout.clear(); // re-enable output to console
    auto viewport = std::make_shared<Viewport>("viewport", 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    scene->Init(CAMERA_ANGLE, DISPLAY_WIDTH, DISPLAY_HEIGHT, NEAR, FAR);
    renderer.AddViewport(scene);
    display.LaunchRendering(true);

    return 0;
}


//#include "BasicScene.h"
//#include "SimpleScene.h"
//
//using namespace cg3d;
//
//int main()
//{
//    const int DISPLAY_WIDTH = 1600;
//    const int DISPLAY_HEIGHT = 800;
//    const float CAMERA_ANGLE = 45.0f;
//    const float NEAR = 0.1f;
//    const float FAR = 120.0f;
//
//    Renderer renderer;
//    Display display("Demo", DISPLAY_WIDTH, DISPLAY_HEIGHT, &renderer);
//    std::cout.setstate(std::ios_base::failbit); // suppress junk output to console from igl::opengl::glfw::Viewer
//    auto scene1 = std::make_shared<SimpleScene>("SimpleScene", &display);
//    auto scene2 = std::make_shared<BasicScene>("BasicScene", &display);
//    std::cout.clear(); // re-enable output to console
//    scene1->Init(CAMERA_ANGLE, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, NEAR, FAR);
//    scene2->Init(CAMERA_ANGLE, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, NEAR, FAR);
//    auto viewport1 = std::make_shared<Viewport>("LeftViewport", 0, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, scene1);
//    auto viewport2 = std::make_shared<Viewport>("RightViewport", DISPLAY_WIDTH / 2, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, scene2);
//    renderer.AddViewport(viewport1);
//    renderer.AddViewport(viewport2);
//    display.LaunchRendering(true);
//
//    return 0;
//}
