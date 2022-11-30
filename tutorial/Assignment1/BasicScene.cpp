#include "BasicScene.h"
#include <read_triangle_mesh.h>
#include <utility>
#include "ObjLoader.h"
#include "IglMeshLoader.h"
#include "igl/read_triangle_mesh.cpp"
#include "igl/edge_flaps.h"

// #include "AutoMorphingModel.h"

using namespace cg3d;

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    camera = Camera::Create( "camera", fov, float(width) / height, near, far);
    
    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")}; 
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();
    this->core().is_animating = true;

 
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material{ std::make_shared<Material>("material", program)}; // empty material
//    SetNamedObject(cube, Model::Create, Mesh::Cube(), material, shared_from_this());
 
    material->AddTexture(0, "textures/box0.bmp", 2);
    auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj")};
    auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","data/cube.off")};
    
    sphere1 = Model::Create( "sphere",sphereMesh, material);
    cube = Model::Create( "cube", cubeMesh, material);
    sphere1->Scale(2);
    sphere1->showWireframe = true;
    sphere1->Translate({-3,0,0});
    cube->showWireframe = true;
    camera->Translate(20, Axis::Z);
    root->AddChild(sphere1);
    root->AddChild(cube);
    
//    auto mesh = cube->GetMeshList();
//    Eigen::VectorXi EMAP;
//    Eigen::MatrixXi F,E,EF,EI;
//    Eigen::VectorXi EQ;
//  // If an edge were collapsed, we'd collapse it to these points:
//    Eigen::MatrixXd V, C;
//    int num_collapsed;
//
//  // Function to reset original mesh and data structures
//    V = mesh[0]->data[0].vertices;
//    F = mesh[0]->data[0].faces;
//   // igl::read_triangle_mesh("data/cube.off",V,F);
//    igl::edge_flaps(F,E,EMAP,EF,EI);
//    std::cout<< "vertices: \n" << V <<std::endl;
//    std::cout<< "faces: \n" << F <<std::endl;
//
//    std::cout<< "edges: \n" << E.transpose() <<std::endl;
//    std::cout<< "edges to faces: \n" << EF.transpose() <<std::endl;
//    std::cout<< "faces to edges: \n "<< EMAP.transpose()<<std::endl;
//    std::cout<< "edges indices: \n" << EI.transpose() <<std::endl;

}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    program.SetUniform4f("lightColor", 1.0f, 1.0f, 1.0f, 0.5f);
    program.SetUniform4f("Kai", 1.0f, 1.0f, 1.0f, 1.0f);
    //cube->Rotate(0.01f, Axis::All);
}

void BasicScene::decreaseQuality() {
    if(pickedModel == nullptr) return;
    if(connectors.find(pickedModel->name) == connectors.end()) {
        initConnectors(pickedModel);
    }
    std::vector<std::shared_ptr<Connector>> modelConnectors = connectors[pickedModel->name];

    std::vector<std::shared_ptr<cg3d::Mesh>> newMeshList;
    for(int i=0; i<pickedModel->GetMeshList().size(); i++) {
        auto newMesh = modelConnectors[i]->simplifyTenPercent(this);
        if(newMesh != nullptr) {
            newMeshList.push_back(newMesh);
        } else {
            newMeshList.push_back(pickedModel->GetMesh(i));
        }
    }
    pickedModel->SetMeshList(newMeshList);

}

void BasicScene::increaseQuality() {
    if(pickedModel == nullptr || connectors.find(pickedModel->name) == connectors.end()) return;
    std::vector<std::shared_ptr<cg3d::Mesh>> newMeshList;
    for(auto connector : connectors[pickedModel->name]) {
        newMeshList.push_back(connector->reset(this));
    }
    pickedModel->SetMeshList(newMeshList);
}

void BasicScene::initConnectors(std::shared_ptr<cg3d::Model> model) {
    std::vector<std::shared_ptr<Connector>> connectorsList;
    for(auto mesh : model->GetMeshList()) {
        auto connector = std::make_shared<Connector>(mesh);
        connector->reset(this);
        connectorsList.push_back(connector);
    }
    connectors[model->name] = connectorsList;

}

void BasicScene::KeyCallback(Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {

        if (key == GLFW_KEY_SPACE) {
            decreaseQuality();
        }
        else if(key == GLFW_KEY_BACKSPACE) {
            increaseQuality();
        }
        else if(key == GLFW_KEY_ENTER)
            draw();
    }
}
