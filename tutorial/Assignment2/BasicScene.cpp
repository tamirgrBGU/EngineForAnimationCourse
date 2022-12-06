#include "BasicScene.h"
#include <read_triangle_mesh.h>
#include <utility>
#include "ObjLoader.h"
#include "IglMeshLoader.h"
#include "igl/read_triangle_mesh.cpp"
#include "igl/edge_flaps.h"
#include "BoundingBox.h"

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

 
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material{ std::make_shared<Material>("material", program)}; // empty material
//    SetNamedObject(cube, Model::Create, Mesh::Cube(), material, shared_from_this());
 
    material->AddTexture(0, "textures/box0.bmp", 2);
    auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj")};
    auto camelMesh{IglLoader::MeshFromFiles("cyl_igl", "data/camel_b.obj")};
    auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","data/cube.off")};
    
    sphere1 = Model::Create( "sphere",sphereMesh, material);
    camel = Model::Create("camel", camelMesh, material);
    cube = Model::Create( "cube", cubeMesh, material);
    sphere1->Scale(2);
    sphere1->showWireframe = true;
    sphere1->Translate({-3,0,0});
    camel->Translate({3, 0, 0});
    //camel->Rotate(30, cg3d::Movable::Axis::Y);
    camel->Scale(0.12f);
    camel->showWireframe = true;
    cube->showWireframe = true;
    camera->Translate(20, Axis::Z);
    root->AddChild(sphere1);
    root->AddChild(camel);
    root->AddChild(cube);
    
    auto mesh = cube->GetMeshList();
    Eigen::VectorXi EMAP;
    Eigen::MatrixXi F,E,EF,EI;
    Eigen::VectorXi EQ;
  // If an edge were collapsed, we'd collapse it to these points:
    Eigen::MatrixXd V, C;
    int num_collapsed;

  // Function to reset original mesh and data structures
    V = mesh[0]->data[0].vertices;
    F = mesh[0]->data[0].faces;
   // igl::read_triangle_mesh("data/cube.off",V,F);
    igl::edge_flaps(F,E,EMAP,EF,EI);
    std::cout<< "vertices: \n" << V <<std::endl;
    std::cout<< "faces: \n" << F <<std::endl;
    
    std::cout<< "edges: \n" << E.transpose() <<std::endl;
    std::cout<< "edges to faces: \n" << EF.transpose() <<std::endl;
    std::cout<< "faces to edges: \n "<< EMAP.transpose()<<std::endl;
    std::cout<< "edges indices: \n" << EI.transpose() <<std::endl;

}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    program.SetUniform4f("lightColor", 1.0f, 1.0f, 1.0f, 0.5f);
    program.SetUniform4f("Kai", 1.0f, 1.0f, 1.0f, 1.0f);
    std::vector<std::shared_ptr<Model>> rootModels;
    for(auto child : root->children) {
        std::shared_ptr<Model> model = std::dynamic_pointer_cast<Model>(child);
        if(model != nullptr && !model->isHidden) {
            rootModels.push_back(model);
        }
    }

    for(int i=0; i < rootModels.size(); i++) {
        for(int j=i+1; j < rootModels.size(); j++) {
            CheckAndHandleCollusion(rootModels[i], rootModels[j]);
        }
    }

    //cube->Rotate(0.01f, Axis::All);
}

std::vector<Eigen::Vector3d> BasicScene::VerticesList(std::shared_ptr<cg3d::Model> model) {
    std::vector<Eigen::Vector3d> points;

   Eigen::Matrix4d transform = model->GetTransform().cast<double>();
    for(auto mesh : model->GetMeshList()) {

        for(const auto& meshData : mesh->data) {
            for(int i=0; i<meshData.vertices.rows(); i++) {
                Eigen::Vector4d vertex(meshData.vertices(i, 0), meshData.vertices(i, 1), meshData.vertices(i, 2), 1);
                Eigen::Vector4d fixedVertex4d = transform * vertex;
                Eigen::Vector3d fixedVertex3d(fixedVertex4d.x(), fixedVertex4d.y(), fixedVertex4d.z());

                points.push_back(fixedVertex3d);
            }
        }
    }
    return points;
}


void BasicScene::CheckAndHandleCollusion(std::shared_ptr<cg3d::Model> model1, std::shared_ptr<cg3d::Model> model2) {
    auto vList1 = VerticesList(model1);
    auto vList2 = VerticesList(model2);

    BoundingBox bb1(vList1);
    BoundingBox bb2(vList2);
    auto collusionRes = bb1.Collide(bb2);
    bool collide = false;

//    if(collusionRes.has_value()) {
//        collide = true;
//    }


    while(collusionRes.has_value()) {
        BoundingBox collusionBox = collusionRes.value();
        int vList1SizeBefore = vList1.size();
        int vList2SizeBefore = vList2.size();
        vList1 = collusionBox.FilterIfOut(vList1);
        vList2 = collusionBox.FilterIfOut(vList2);
//        if(vList1.size() < 10 && vList2.size() < 10) {
//            collide = true;
//            break;
//        }
        if(vList1.size() < 2 || vList2.size() < 2) {
            collide = true;
            break;
        }
        if(vList1SizeBefore == vList1.size() && vList2SizeBefore == vList2.size()) {
            collide = true;
            break;
        }
        bb1 = BoundingBox(vList1);
        bb2 = BoundingBox(vList2);
        collusionRes = bb1.Collide(bb2);
    }

    if(collide) {
        HandleCollusion(model1, model2);
    }

}

void BasicScene::HandleCollusion(std::shared_ptr<cg3d::Model> model1, std::shared_ptr<cg3d::Model> model2) {
    std::cout << "Collusion detected between " << model1->name << " and " << model2->name << "!" << std::endl;
}
