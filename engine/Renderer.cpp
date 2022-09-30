#include "Renderer.h"
#include "Shape.h"
#include <GLFW/glfw3.h>

#include <igl/unproject_onto_mesh.h>
#include <memory>
#include <utility>

#include "Debug.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

using namespace std;

Renderer::Renderer()
{
    callback_init = nullptr;
    callback_pre_draw = nullptr;
    callback_post_draw = nullptr;
    callback_mouse_down = nullptr;
    callback_mouse_up = nullptr;
    callback_mouse_move = nullptr;
    callback_mouse_scroll = nullptr;
    callback_key_down = nullptr;
    callback_key_up = nullptr;

    callback_init_data = nullptr;
    callback_pre_draw_data = nullptr;
    callback_post_draw_data = nullptr;
    callback_mouse_down_data = nullptr;
    callback_mouse_up_data = nullptr;
    callback_mouse_move_data = nullptr;
    callback_mouse_scroll_data = nullptr;
    callback_key_down_data = nullptr;
    callback_key_up_data = nullptr;
    callback_key_pressed_data = nullptr;

    glLineWidth(5);
    isPressed = false;
    isMany = false;
    xold = 0;
    yold = 0;
    xrel = 0;
    yrel = 0;
    zrel = 0;
    currentViewport = 0;
    isPicked = false;
}

void Renderer::Init(Scene* _scene, list<int> xViewport, list<int> yViewport, const shared_ptr<Camera>& camera, int pickingBits)
{
    scene = _scene;
    //menu = _menu;
    Vector4i viewport;
    glGetIntegerv(GL_VIEWPORT, viewport.data());
    buffers.push_back(new DrawBuffer());
    xViewport.push_front(0);
    yViewport.push_front(0);
	//menu = _menu;
	// Vector4i viewport(0,0,xViewport.back(),yViewport.back());
	
	// glGetIntegerv(GL_VIEWPORT, viewport.data());
	buffers.push_back(new DrawBuffer());
	// maxPixX = xViewport.back();
	// maxPixY = yViewport.back();

    auto xit = xViewport.begin();
    int viewportIndex = 0;

    // TODO: TAL: bug here when more than 1 viewport? (for nesting seems strange)
    for (++xit; xit != xViewport.end(); ++xit) {
        auto yit = yViewport.begin();
        for (++yit; yit != yViewport.end(); ++yit) {
            viewports.emplace_back(*prev(xit), *prev(yit), *xit - *prev(xit), *yit - *prev(yit));

            if ((1 << viewportIndex) & pickingBits) {
                auto drawInfo = new DrawInfo(viewportIndex, camera, 0, 0,
                                             1 | inAction | depthTest | stencilTest | passStencil | blackClear | clearStencil | clearDepth | onPicking);
                drawInfos.emplace_back(drawInfo);
            }

            auto temp = new DrawInfo(viewportIndex, camera, 1, 0, (int) (viewportIndex < 1) | depthTest | clearDepth);
            drawInfos.emplace_back(temp);
            viewportIndex++;
        }
    }

    //if (menu)
    //{
    //    menu->callback_draw_viewer_menu = [&]()
    //    {
    //        // Draw parent menu content
    //        auto temp = Vector4i(0, 0, 0, 0); // set imgui to min size and top left corner
    //        menu->draw_viewer_menu(scene, cameras, temp, drawInfos);
    //    };
    //}
}

void Renderer::Clear(float r, float g, float b, float a, unsigned int flags)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT & flags);
}

void Renderer::DrawByInfo(int infoIndex)
{
    DrawInfo* info = drawInfos[infoIndex];
    buffers[info->buffer]->Bind();
    glViewport(viewports[info->viewport].x(), viewports[info->viewport].y(), viewports[info->viewport].z(), viewports[info->viewport].w());
    if (info->flags & scissorTest) {
        glEnable(GL_SCISSOR_TEST);
        int x = min(xWhenPress, xold);
        int y = min(viewports[info->viewport].w() - yWhenPress, viewports[info->viewport].w() - yold);
        glScissor(x, y, abs(xWhenPress - xold), abs(yWhenPress - yold));
    } else
        glDisable(GL_SCISSOR_TEST);

    if (info->flags & stencilTest) {
        glEnable(GL_STENCIL_TEST);
        if (info->flags & passStencil) {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
        } else {
            if (info->flags & stencil2) {
                glStencilFunc(GL_EQUAL, 1, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
            } else {
                glStencilFunc(GL_EQUAL, 0, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            }
        }
    } else
        glDisable(GL_STENCIL_TEST);

    if (info->flags & depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    if (info->flags & blend) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    } else
        glDisable(GL_BLEND);

    if (info->flags & toClear) {
        if (info->flags & blackClear)
            Clear(0, 0, 0, 0, info->flags);
        else
            Clear(info->clearColor.x(), info->clearColor.y(), info->clearColor.z(), info->clearColor.w(), info->flags);
    }

    if (info->flags & onPicking) {
        pickingData = std::make_unique<Movable::PickingData>();
        scene->DrawForPicking(info->viewport, info->camera, pickingData.get());
    } else {
        scene->Draw(info->viewport, info->camera, info->flags);
    }
}

void Renderer::Draw(GLFWwindow* window)
{
    using namespace std;
    using namespace Eigen;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    int width_window, height_window;
    glfwGetWindowSize(window, &width_window, &height_window);

    auto highdpi_tmp = (width_window == 0 || width == 0) ? highdpi : float(width) / float(width_window);

    if (fabs(highdpi_tmp - highdpi) > 1e-8) {
        PostResize(window, width, height);
        highdpi = highdpi_tmp;
    }

    //if (menu) {
    //	menu->pre_draw();
    //	menu->callback_draw_viewer_menu();
    //}

    int index = 0;
    for (auto& info: drawInfos) {
        if (!(info->flags & (inAction | inAction2)) ||
            ((info->flags & inAction2) && !(info->flags & stencilTest) && isPressed && !isPicked) ||
            ((info->flags & inAction2) && (info->flags & stencilTest) && isPicked))
            DrawByInfo(index);
        index++;
    }

    //if (menu) {
    //	menu->post_draw();
    //}
}

void Renderer::UpdatePosition(double xpos, double ypos)
{
    xrel = (int) (xold - xpos);
    yrel = (int) (yold - ypos);
    xold = (int) xpos;
    yold = (int) ypos;
}

void Renderer::UpdatePress(double xpos, double ypos)
{
    xWhenPress = (int) xpos;
    yWhenPress = (int) ypos;
}

/*
void Renderer::AddViewport(int left, int bottom, int width, int height)
{
    viewports.emplace_back(Vector4i(left, bottom, width, height));
    glViewport(left, bottom, width, height);

}

void Renderer::AddDraw(int viewportIndex, int cameraIndex, int shaderIndex, int buffIndex, unsigned int flags)
{
    drawInfos.emplace_back(new DrawInfo(viewportIndex, shaderIndex, buffIndex, flags));
}

void Renderer::CopyDraw(int infoIndex, int property, int index)
{
    DrawInfo* info = drawInfos[infoIndex];
    switch (property) {// NOLINT(hicpp-multiway-paths-covered)
        case non:
            drawInfos.emplace_back(new DrawInfo(info->viewport, info->shader, info->buffer, info->flags, info->camera));
            break;
        case viewport:
            drawInfos.emplace_back(new DrawInfo(index, info->shader, info->buffer, info->flags, info->camera));
            break;
        case camera: // TODO: TAL: i broke this because camera is now an object
            drawInfos.emplace_back(new DrawInfo(info->viewport, info->shader, info->buffer, info->flags, info->camera));
            break;
        case shader:
            drawInfos.emplace_back(new DrawInfo(info->viewport, index, info->buffer, info->flags, info->camera));
            break;
        case buffer:
            drawInfos.emplace_back(new DrawInfo(info->viewport, info->shader, index, info->flags, info->camera));
            break;
    }
}

void Renderer::SetViewport(int left, int bottom, int width, int height, int index)
{
    viewports[index] = Vector4i(left, bottom, width, height);
    glViewport(left, bottom, width, height);
}

*/

bool Renderer::PickObject(int x, int y)
{
    unsigned char pixelData[4]{0, 0, 0, 0};
    int viewport = 0;
    for (; viewport < viewports.size() && !CheckViewport(x, y, viewport); viewport++);
    ActionDraw(viewport);
    glReadPixels(x, viewports[viewport].w() - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glReadPixels(x, viewports[viewport].w() - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    scene->Pick(pixelData, pickingData.get());
    return scene->pickedObject != nullptr;
}

void Renderer::OutLine()
{
    ActionDraw(0);
}

/*
void Renderer::PickMany(int viewport)
{
	if (!isPicked) {
		int viewportCurrIndx = 0;
		int xMin = min(xWhenPress, xold);
		int yMin = min(viewports[viewportCurrIndx].w() - yWhenPress, viewports[viewportCurrIndx].w() - yold);
		int xMax = max(xWhenPress, xold);
		int yMax = max(viewports[viewportCurrIndx].w() - yWhenPress, viewports[viewportCurrIndx].w() - yold);
		depth = scene->AddPickedShapes(
				cameras[0]->GetViewProjection().cast<double>() * (cameras[0]->MakeTransf()).inverse(),
				viewports[viewportCurrIndx], viewportCurrIndx, xMin, xMax, yMin, yMax, viewport);
		if (depth != -1) {
			depth = (depth * 2.0f - cameras[0]->GetFar()) / (cameras[0]->GetNear() - cameras[0]->GetFar());
			isMany = true;
			isPicked = true;
		} else
			depth = 0;

	}
}
*/

void Renderer::ActionDraw(int viewportIndex)
{
    //if (menu)
    //{
    //    menu->pre_draw();
    //    menu->callback_draw_viewer_menu();
    //}
    for (int i = 0; i < drawInfos.size(); i++) {
        if ((drawInfos[i]->flags & inAction) && viewportIndex == drawInfos[i]->viewport)
            DrawByInfo(i);
    }
    //if (menu)
    //    menu->post_draw();
}

void Renderer::Resize(GLFWwindow* window, int w, int h)
{
    PostResize(window, w, h);
}

void Renderer::PostResize(GLFWwindow* window, int w, int h)
{
    // hold old windows size
    int x = 0;
    int y = 0;
    for (auto& viewport: viewports) {
        x = max(x, viewport.x() + viewport.z());
        y = max(y, viewport.y() + viewport.w());
    }
    float ratio_x = (float) w / (float) x;
    float ratio_y = (float) h / (float) y;
    for (auto& viewport: viewports) {
        viewport = Vector4i((int) ((float) viewport.x() * ratio_x), (int) ((float) viewport.y() * ratio_y),
                            (int) ((float) viewport.z() * ratio_x), (int) ((float) viewport.w() * ratio_y));
    }
    if (callback_post_resize) {
        callback_post_resize(window, w, h);
    }
}

bool Renderer::CheckViewport(int x, int y, int viewportIndex)
{
    return (viewports[viewportIndex].x() < x && viewports[viewportIndex].y() < y &&
            viewports[viewportIndex].z() + viewports[viewportIndex].x() > x &&
            viewports[viewportIndex].w() + viewports[viewportIndex].y() > y);
}

/*
bool Renderer::UpdateViewport(int viewport)
{
    if (viewport != currentViewport) {
        isPicked = false;
        currentViewport = viewport;
        Pressed();
        scene->UnPick();
        return false;
    }
    return true;
}
*/

void Renderer::MouseProcessing(int button, int viewport, const shared_ptr<Camera>& camera)
{
    int width = viewports[viewport].w();
    scene->MouseProcessing(button, xrel, yrel, depth, width, camera);
}

void Renderer::SetViewportCamera(int viewport, const shared_ptr<Camera>& camera)
{
    for (auto& drawInfo : drawInfos)
        if (drawInfo->viewport == viewport)
            drawInfo->camera = camera;
}

/*
unsigned int Renderer::AddBuffer(int infoIndex)
{
    CopyDraw(infoIndex, buffer, int(buffers.size()));

    DrawInfo* info = drawInfos.back();
    info->SetFlags(stencilTest);
    // info->ClearFlags(depthTest);
    info->SetFlags(clearDepth | clearStencil);
    int width = viewports[info->viewport].z(), height = viewports[info->viewport].w();

    unsigned int texId;
//    texId = scene->AddTexture(width, height, 0, COLOR);
//    scene->AddTexture(width, height, 0, DEPTH);
//    scene->BindTexture(texId, texId);
    buffers.push_back(new DrawBuffer(width, height, texId));

    return texId;
}

void Renderer::SetBuffers()
{
    AddCamera(Vector3f(0, 0, 1), 0, 1, 1, 10,2);
    int materialIndx = Create2Dmaterial(1,1);
    scene->SetShapeMaterial(6, materialIndx);
    SwapDrawInfo(2, 3);
}
*/