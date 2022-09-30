#include "Scene.h"

#include <cmath>
#include <thread>
#include <writeOFF.h>
#include "GLFW/glfw3.h"

#include "Debug.h"
#include "Camera.h"

using namespace std;
using std::make_shared;
using namespace Eigen;

Scene::Scene(const string& name) : Movable(name) // , isActive(false)
{
    //shapes.front() = new ViewerData(); // this was for the overlays
    //shapes.front()->id = 0;

    staticScene = 0;
    overlay_point_program = nullptr;
    overlay_program = nullptr;

    // Temporary variables initialization
    scroll_position = 0.0f;
}

void Scene::Draw(int viewport, const shared_ptr<Camera>& camera, unsigned int flags)
{
    Matrix4f proj = camera->GetViewProjection();
    Matrix4f view = camera->GetTransform().inverse();
    Matrix4f norm = !(staticScene & (1 << viewport)) ? GetTransform() : Matrix4f::Identity();
    Movable::Draw(proj, view, norm, viewport, flags);
}

void Scene::DrawForPicking(int viewport, const shared_ptr<Camera>& camera, PickingData* pickingData)
{
    Matrix4f proj = camera->GetViewProjection();
    Matrix4f view = camera->GetTransform().inverse();
    Matrix4f norm = !(staticScene & (1 << viewport)) ? GetTransform() : Matrix4f::Identity();
    Movable::DrawForPicking(proj, view, norm, viewport, pickingData);
}

/*
void Scene::ClearPickedShapes(int viewportIndex)
{
//    for (int pShape: pShapes) {
//        shapes[pShape]->RemoveViewport(viewport);
//    }
    selected_data_index = -1;
    pShapes.clear();
}
*/

void Scene::MouseProcessing(int button, int xDiff, int yDiff, float depth, int width, const shared_ptr<Camera>& camera) const
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        auto moveCoeff = camera->CalcMoveCoeff(depth, width);
        if (pickedObject) {
            pickedObject->Translate(-float(xDiff) / moveCoeff, Axis::X);
            pickedObject->Translate(float(yDiff) / moveCoeff, Axis::Y);
        } else {
            // TODO: TAL: why ffs do i need to rotate Axis:X with yDiff and vice-versa???
            camera->Translate(-float(xDiff), Axis::Y);
            camera->Translate(-float(yDiff), Axis::X);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (pickedObject) {
            pickedObject->Rotate(float(xDiff) / 180.0f * M_PI, Axis::Z);
            pickedObject->Rotate(float(yDiff) / 180.0f * M_PI, Axis::X);
        } else {
            // TODO: TAL: why ffs do i need to rotate Axis:X with yDiff and vice-versa???
            camera->Rotate(-float(xDiff) / 180.0f * M_PI, Axis::Z);
            camera->Rotate(-float(yDiff) / 180.0f * M_PI, Axis::X);
        }
    }
}

void Scene::Pick(const unsigned char data[4], PickingData *pickingData)
{
    int pickedObjectId = data[0] + data[1] * 256 + data[2] * 256 * 256;
	
	pickedObject = pickedObjectId > 0 ? pickingData->objects[pickedObjectId - 1] : nullptr;

    debug("object picked id: ", pickedObjectId, " name: ", pickedObject ? pickedObject->name : "none");
}

void Scene::WhenTranslate(const Matrix4f& preMat, float dx, float dy)
{
    Movable* obj = nullptr; // TODO: TAL
//    if (selected_data_index == -1 || shapes[selected_data_index]->IsStatic())
//        obj = (Movable *) this;
//    else
//        obj = shapes[selected_data_index].get();

    obj->TranslateInSystem(preMat.block<3, 3>(0, 0), Vector3f(dx, 0, 0));
    obj->TranslateInSystem(preMat.block<3, 3>(0, 0), Vector3f(0, dy, 0));
    WhenTranslate(dx, dy);
}

/*
void Scene::WhenRotate(const Matrix4f &preMat, float dx, float dy)
{
    Movable *obj;
    if (selected_data_index == -1 || shapes[selected_data_index]->IsStatic())
        obj = (Movable *) this;
    else {
        int ps = selected_data_index;
        for (; parents[ps] > -1; ps = parents[ps]);
        obj = shapes[ps].get();
    }
    obj->RotateInSystem(dx, Axis::X);
    obj->RotateInSystem(dy, Axis::Y);
    WhenRotate(dx, dy);
}
*/

void Scene::WhenScroll(const Matrix4f& preMat, float dy)
{
//    if (selected_data_index == -1 || shapes[selected_data_index]->IsStatic())
        this->TranslateInSystem(preMat.block<3, 3>(0, 0), Vector3f(0, 0, dy));
//    else
//        shapes[selected_data_index]->TranslateInSystem(preMat.block<3, 3>(0, 0), Vector3f(0, 0, dy));
    WhenScroll(dy);
}

/*float
Scene::AddPickedShapes(const Matrix4f &PV, const Vector4i &viewport, int viewport, int left, int right, int up,
                        int bottom, int newViewportIndx)
{
    //not correct when the shape is scaled
    Matrix4f MVP = PV * MakeTransd();
    std::cout << "picked shapes  ";
    bool isFound = false;
    for (int i = 1; i < shapes.size(); i++) { //add to pShapes if the center in range
        Matrix4f Model = shapes[i]->MakeTransd();
        // Model = CalcParentsTrans(i) * Model; TODO: TAL: check
        Vector4f pos = MVP * Model * Vector4d(0, 0, 0, 1);
        float xpix = (1 + pos.x() / pos.z()) * viewport.z() / 2;
        float ypix = (1 + pos.y() / pos.z()) * viewport.w() / 2;
        if (shapes[i]->Is2Render(viewport) && xpix < right && xpix > left && ypix < bottom && ypix > up) {
            pShapes.push_back(i);
            shapes[i]->AddViewport(newViewportIndx);
            std::cout << i << ", ";
            selected_data_index = i;
            isFound = true;
        }
    }
    std::cout << std::endl;
    if (isFound) {
        Vector4f tmp = MVP * GetPreviousTrans(Matrix4f::Identity(), selected_data_index) *
                       shapes[selected_data_index]->MakeTransf() * Vector4f(0, 0, 1, 1);
        return (float) tmp.z();
    } else
        return -1;
}*/

void Scene::UpdateOverlay(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, unsigned int shapeIndex,
                          bool is_points)
{
//    auto data = shapes[shapeIndex];
//    Program *p = is_points ? overlay_point_program : overlay_program;
//    if (p != nullptr) {
//        p->BindColorBuffer();
//        p->SetUniformMatrix4f("Proj", &Proj);
//        p->SetUniformMatrix4f("View", &View);
//        p->SetUniformMatrix4f("Model", &Model);
//    }
}
