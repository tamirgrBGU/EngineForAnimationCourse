#pragma once

#include <igl_inline.h>
#include <vector>
#include <functional>
#include "DrawInfo.h"
#include "Scene.h"
#include "DrawBuffer.h"
#include "Camera.h"
#include "ViewerData.h"

struct GLFWwindow;

using namespace Eigen;

class Renderer
{
public:
    Renderer();

    enum drawFlags
    {
        toClear = 1,
        is2D = 2,
        inAction = 4,
        scissorTest = 8,
        depthTest = 16,
        stencilTest = 32,
        blend = 64,
        blackClear = 128,
        clearDepth = 256,
        backdraw = 512,
        clearStencil = 1024,
        passStencil = 2048,
        inAction2 = 4096,
        scaleAbit = 8192,
        clearColor = 16384,
        stencil2 = 32768,
        onPicking = 65536
    };
    enum
    {
        preRot, postRot, phiRot, thetaRot, psiRot, psiPhiRot
    };
    enum
    {
        non, viewport, camera, shader, buffer,
    };

    ~Renderer() = default;

    void Draw(GLFWwindow* window);

    // Returns **true** if action should be cancelled.
    std::function<bool(GLFWwindow *window)> callback_init;
    std::function<bool(GLFWwindow *window)> callback_pre_draw;
    std::function<bool(GLFWwindow *window)> callback_post_draw;
    std::function<bool(GLFWwindow *window, int button, int modifier)> callback_mouse_down;
    std::function<bool(GLFWwindow *window, int button, int modifier)> callback_mouse_up;
    std::function<bool(GLFWwindow *window, int mouse_x, int mouse_y)> callback_mouse_move;
    std::function<bool(GLFWwindow *window, float delta_y)> callback_mouse_scroll;
    std::function<bool(GLFWwindow *window, unsigned int key, int modifiers)> callback_key_pressed;
    std::function<bool(GLFWwindow *window, int w, int h)> callback_post_resize;
    // THESE SHOULD BE DEPRECATED:
    std::function<bool(GLFWwindow *window, unsigned int key, int modifiers)> callback_key_down;
    std::function<bool(GLFWwindow *window, unsigned int key, int modifiers)> callback_key_up;
    // Pointers to per-callback data
    void *callback_init_data;
    void *callback_pre_draw_data;
    void *callback_post_draw_data;
    void *callback_mouse_down_data;
    void *callback_mouse_up_data;
    void *callback_mouse_move_data;
    void *callback_mouse_scroll_data;
    void *callback_key_pressed_data;
    void *callback_key_down_data;
    void *callback_key_up_data;

    void Resize(GLFWwindow *window, int w, int h); // explicitly set window size
    void PostResize(GLFWwindow *window, int w, int h); // external resize due to user interaction

    inline Scene *GetScene() { return scene; }

    void UpdatePosition(double xpos, double ypos);

    void UpdatePress(double xpos, double ypos);

    void AddViewport(int left, int bottom, int width, int height);

    unsigned int AddBuffer(int infoIndex);

    void AddDraw(int viewportIndex, int cameraIndex, int shaderIndex, int buffIndex, unsigned int flags);

    void CopyDraw(int infoIndex, int property, int index);

    void SetViewport(int left, int bottom, int width, int height, int index);

    inline void BindViewport2D(int indx) { drawInfos[indx]->SetFlags(is2D); }

    bool PickObject(int x, int y);

    void OutLine();

    void PickMany(int viewportIndx);

    void MouseProcessing(int button, int viewport, const shared_ptr<Camera>& camera);

    inline void SetDrawFlag(int infoIndex, unsigned int flag) { drawInfos[infoIndex]->SetFlags(flag); }

    inline void ClearDrawFlag(int infoIndex, unsigned int flag) { drawInfos[infoIndex]->ClearFlags(flag); }

    inline void Pressed() { isPressed = !isPressed; }

    inline bool IsPressed() const { return isPressed; }

    inline void FreeShapes(int viewportIndex) { /*scene->ClearPickedShapes(viewport);*/ };

    bool CheckViewport(int x, int y, int viewportIndex);

    bool UpdateViewport(int viewport);

    inline size_t GetViewportsSize() { return viewports.size(); }

    inline void UpdateZpos(int ypos) { zrel = ypos; }

    inline void ClearPickedShapes(int viewportIndex)
    {
//        scene->ClearPickedShapes(viewport);
        isMany = false;
    }

    inline void UnpickObjects() { scene->pickedObject = nullptr; }

    inline bool IsMany() const { return isMany; }

    void Init(Scene* _scene, list<int> xViewport, list<int> yViewport, const shared_ptr<Camera>& camera, int pickingBits);

    void SetViewportCamera(int viewport, const shared_ptr<Camera>& camera);

private:
    unique_ptr<Movable::PickingData> pickingData = nullptr;

protected:

    void DrawByInfo(int infoIndex = 1);
    void ActionDraw(int viewportIndex);

    static void Clear(float r, float g, float b, float a, unsigned int flags);

    // Stores all the viewing options
    Scene *scene{};
    std::vector<Vector4i> viewports;
    std::vector<DrawInfo *> drawInfos;
    std::vector<DrawBuffer *> buffers;
    int xold, yold, xrel, yrel, zrel;
    int xWhenPress{}, yWhenPress{};
    bool isMany;
    bool isPicked;
    bool isPressed;
    int currentViewport;
    float highdpi{};
    float depth{};
    //imgui::ImGuiMenu* menu;
    double z{};
    shared_ptr<Movable> pickedShape;
};
