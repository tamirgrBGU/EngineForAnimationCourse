
#include "igl/opengl/glfw/renderer.h"
#include "tutorial/IK/inputManager.h"
#include "IK.h"

int main(int argc, char *argv[])
{
  Display *disp = new Display(1000, 800, "Wellcome");
  Renderer renderer;
  IK viewer;
  viewer.Init("configuration.txt");
  Init(*disp);
  renderer.init(&viewer,2,0);
  disp->SetRenderer(&renderer);
  disp->launch_rendering(true);
  
  delete disp;
}
