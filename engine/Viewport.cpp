#include "Viewport.h"
#include <GL.h>


namespace cg3d
{

void Viewport::Bind() const
{
    glViewport(x, y, width, height);
}

bool Viewport::IsInside(int _x, int _y) const
{
    _y = height - _y; // window coordinate y goes upwards

    return _x >= x && _y >= y && _x <= x + width && _y <= y + height;
}

void Viewport::Resize(int oldWidth, int oldHeight, int newWidth, int newHeight)
{
    x = x * newWidth / oldWidth;
    width = width * newWidth / oldWidth;
    y = y * newHeight / oldHeight;
    height = height * newHeight / oldHeight;
}

} // namespace cg3d
