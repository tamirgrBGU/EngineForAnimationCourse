#include "SceneWithCameras.h"

#include <utility>

#include "ObjLoader.h"
#include "AutoMorphingModel.h"
#include "SceneWithImGui.h"
#include "CamModel.h"
#include "Visitor.h"
#include "Debug.h"

#include "imgui.h"
#include "file_dialog_open.h"
#include "GLFW/glfw3.h"

using namespace cg3d;

void SceneWithCameras::BuildImGui()
{
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    if (ImGui::Button("Load object"))
        LoadObjectFromFileDialog();

    ImGui::Text("Camera: ");
    for (int i = 0; i < camList.size(); i++) {
        ImGui::SameLine(0);
        bool selectedCamera = camList[i] == camera;
        if (selectedCamera)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        if (ImGui::Button(std::to_string(i + 1).c_str()))
            SetCamera(i);
        if (selectedCamera)
            ImGui::PopStyleColor();
    }
    ImGui::SameLine();
    if (ImGui::Button("Center"))
        camera->SetTout(Eigen::Affine3f::Identity());
    if (pickedModel) {
        ImGui::Text("Picked model: %s", pickedModel->name.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Drop"))
            pickedModel = nullptr;
        if (pickedModel) {
            if (ImGui::CollapsingHeader("Draw options", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Show wireframe", &pickedModel->showWireframe);
                if (pickedModel->showWireframe) {
                    ImGui::Text("Wireframe color:");
                    ImGui::SameLine();
                    ImGui::ColorEdit4("Wireframe color", pickedModel->wireframeColor.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                }
                ImGui::Checkbox("Show faces", &pickedModel->showFaces);
                ImGui::Checkbox("Show textures", &pickedModel->showTextures);
                if (ImGui::Button("Scale down"))
                    pickedModel->Scale(0.9f);
                ImGui::SameLine();
                if (ImGui::Button("Scale up"))
                    pickedModel->Scale(1.1f);
            }
            if (ImGui::Button("Dump model mesh data")) {
                std::cout << "model name: " << pickedModel->name << std::endl;
                if (pickedModel->meshIndex > 0)
                    std::cout << "mesh index in use: " << pickedModel->meshIndex;
                for (auto& mesh: pickedModel->GetMeshList()) {
                    Eigen::IOFormat simple(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "", "");
                    std::cout << "mesh name: " << mesh->name << std::endl;
                    for (int i = 0; i < mesh->data.size(); i++) {
                        if (mesh->data.size() > 1)
                            std::cout << "mesh #" << i + 1 << ":" << std::endl;
                        DumpMeshData(simple, mesh->data[i]);
                    }
                }
            }
            if (ImGui::Button("Dump model transformations")) {
                Eigen::IOFormat format(2, 0, ", ", "\n", "[", "]");
                const Eigen::Matrix4f& transform = pickedModel->GetAggregatedTransform();
                std::cout << "Tin:" << std::endl << pickedModel->Tin.matrix().format(format) << std::endl
                          << "Tout:" << std::endl << pickedModel->Tout.matrix().format(format) << std::endl
                          << "Transform:" << std::endl << transform.matrix().format(format) << std::endl
                          << "--- Transform Breakdown ---" << std::endl
                          << "Rotation:" << std::endl << Movable::GetTranslation(transform).matrix().format(format) << std::endl
                          << "Translation:" << std::endl << Movable::GetRotation(transform).matrix().format(format) << std::endl
                          << "Rotation x Translation:" << std::endl << Movable::GetTranslationRotation(transform).matrix().format(format)
                          << std::endl << "Scaling:" << std::endl << Movable::GetScaling(transform).matrix().format(format) << std::endl;
            }
        }
    }

    ImGui::End();
}

void SceneWithCameras::DumpMeshData(const Eigen::IOFormat& simple, const MeshData& data)
{
    std::cout << "vertices mesh: " << data.vertices.format(simple) << std::endl;
    std::cout << "faces mesh: " << data.faces.format(simple) << std::endl;
    std::cout << "vertex normals mesh: " << data.vertexNormals.format(simple) << std::endl;
    std::cout << "texture coordinates mesh: " << data.textureCoords.format(simple) << std::endl;
}

SceneWithCameras::SceneWithCameras(std::string name, Display* display) : SceneWithImGui(std::move(name), display)
{
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
}

void SceneWithCameras::SetCamera(int index)
{
    camera = camList[index];
    viewport->camera = camera;
}

void SceneWithCameras::Init(float fov, int width, int height, float near, float far)
{
    // create the basic elements of the scene
    SetNamedObject(root, Movable::Create, shared_from_this()); // the parent of all the shapes
    auto program = std::make_shared<Program>("shaders/basicShader"); // TODO: TAL: replace with hard-coded basic program
    SetNamedObject(carbon, std::make_shared<Material>, program); // default material
    carbon->AddTexture(0, "textures/carbon.jpg", 2);

    // create the camera objects
    camList.resize(camList.capacity());
    camList[0] = std::make_shared<Camera>("camera0", fov, float(width) / float(height), near, far);
    for (int i = 1; i < camList.size(); i++) {
        auto camera = Camera("", fov, double(width) / height, near, far);
        auto model = ObjLoader::ModelFromObj(std::string("camera") + std::to_string(i), "data/camera.obj", carbon);
        camList[i] = std::make_shared<CamModel>(camera, *model);
        camList[i]->SetParent(root);
    }

    camList[0]->Translate(10, Axis::Z);
    camList[1]->Translate(-3, Axis::X);
    camList[1]->RotateByDegree(-90, Axis::Y);
    camList[2]->Translate(-8, Axis::Z);
    camList[2]->RotateByDegree(180, Axis::Y);
    camList[3]->Translate(3, Axis::X);
    camList[3]->RotateByDegree(90, Axis::Y);
    camera = camList[0];

    NewNamedObject(bricks, std::make_shared<Material>, program);
    NewNamedObject(grass, std::make_shared<Material>, program);
    NewNamedObject(daylight, std::make_shared<Material>, "shaders/cubemapShader");

    bricks->AddTexture(0, "textures/bricks.jpg", 2);
    grass->AddTexture(0, "textures/grass.bmp", 2);
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);

    NewNamedObject(background, Model::Create, Mesh::Cube(), daylight, root);
    SetNamedObject(cube1, Model::Create, Mesh::Cube(), bricks, root);
    SetNamedObject(cube2, Model::Create, Mesh::Cube(), bricks, root);

    cube1->Translate({-3, 0, -5});
    cube2->Translate({3, 0, -5});

    NewNamedObject(snakeMesh, ObjLoader::MeshFromObjFiles<std::string>, "data/snake1.obj", "data/snake2.obj");
    NewNamedObject(blank, std::make_shared<Material>, program);
    NewNamedObject(snake, Model::Create, snakeMesh, blank);

    auto morphFunc = [](Model* model, Visitor* visitor) {
        static float prevDistance = -1;
        float distance = (visitor->view * visitor->norm * model->aggregatedTransform).norm();
        if (prevDistance != distance)
            debug(model->name, " distance from camera: ", prevDistance = distance);
        return distance > 3 ? 1 : 0;
    };
    auto autoSnake = AutoMorphingModel::Create(*snake, morphFunc, root.get());
    autoSnake->showWireframe = true;
    autoSnake->SetParent(root);

    SetNamedObject(cylinder, Model::Create, Mesh::Cylinder(), grass, root);
    NewNamedObject(sphereMesh, ObjLoader::MeshFromObjFiles, "data/sphere.obj");
    SetNamedObject(sphere1, Model::Create, sphereMesh, grass, cylinder);
    SetNamedObject(sphere2, Model::Create, sphereMesh, grass, cylinder);
    sphere1->Translate(-1.3, Axis::X);
    sphere2->Translate(1.3, Axis::X);
    sphere1->showWireframe = true;
    sphere2->showWireframe = true;
    sphere1->isPickable = false;
    sphere2->isPickable = false;
    cylinder->Translate({0, -3, -5});
    cylinder->showWireframe = true;

    background->Scale(120, Axis::All);
    background->SetPickable(false);
    background->SetStatic();
}

std::shared_ptr<CamModel> SceneWithCameras::CreateCameraWithModel(int width, int height, float fov, float near, float far, const std::shared_ptr<Material>& material)
{
    static CamModel prototype([width, height, fov, near, far, material]() -> CamModel {
        auto camera = Camera("", fov, double(width) / height, near, far);
        auto model = ObjLoader::ModelFromObj("camera", "data/camera.obj", material);
        return {camera, *model};
    }());

    static int counter = 0;

    auto camModel = std::make_shared<CamModel>(prototype);
    camModel->name += std::to_string(++counter);
    return camModel;
}

void SceneWithCameras::Update(const Program& p, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(p, proj, view, model);

    if (animate) {
        cube1->Rotate(0.003f, {1, 1, 0});
        cube2->Rotate(0.003f, {0, 1, 1});
        camList[0]->Rotate(0.001f, Axis::Y);
    }
}

void SceneWithCameras::LoadObjectFromFileDialog()
{
    std::string filename = igl::file_dialog_open();
    if (filename.length() == 0) return;

    auto shape = Model::Create(filename, carbon, root);
}

void SceneWithCameras::KeyCallback(Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_SPACE)
            SetActive(!IsActive());

        // keys 1-9 are objects 1-9 (objects[0] - objects[8]), key 0 is object 10 (objects[9])
        if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
            if (int index; (index = (key - GLFW_KEY_1 + 10) % 10) < camList.size())
                SetCamera(index);
        }
    }

    SceneWithImGui::KeyCallback(nullptr, x, y, key, scancode, action, mods);
}

void SceneWithCameras::ViewportSizeCallback(Viewport* _viewport)
{
    for (auto& cam: camList)
        cam->SetProjection(float(_viewport->width) / float(_viewport->height));

    // note: we don't need to call Scene::ViewportSizeCallback since we are setting the projection of all the cameras
}

void SceneWithCameras::AddViewportCallback(Viewport* _viewport)
{
    viewport = _viewport;

    Scene::AddViewportCallback(viewport);
}
