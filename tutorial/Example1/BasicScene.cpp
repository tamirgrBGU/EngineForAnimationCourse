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
    SetNamedObject(camera, std::make_shared<Camera>, fov, float(width) / height, near, far);
    NewNamedObject(root, Movable::Create, shared_from_this());

    NewNamedObject(daylight, std::make_shared<Material>, "shaders/cubemapShader");
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    NewNamedObject(background, Model::Create, Mesh::Cube(), daylight, root);
    background->Scale(120, Axis::All);
    background->SetPickable(false);
    background->SetStatic();

 
    auto program = std::make_shared<Program>("shaders/basicShader");
    NewNamedObject(material, std::make_shared<Material>, program); // empty material
//    SetNamedObject(cube, Model::Create, Mesh::Cube(), material, shared_from_this());
 
    material->AddTexture(0, "textures/box0.bmp", 2);
    NewNamedObject(sphereMesh, IglLoader::MeshFromFiles, "data/sphere.obj");
    NewNamedObject(cylMesh, IglLoader::MeshFromFiles, "data/camel_b.obj");
    NewNamedObject(cubeMesh, IglLoader::MeshFromFiles, "data/cube.off");
    
    SetNamedObject(sphere1, Model::Create, sphereMesh, material,shared_from_this());
    SetNamedObject(cyl, Model::Create, cylMesh, material,shared_from_this());
    SetNamedObject(cube, Model::Create, cubeMesh, material,shared_from_this());
    sphere1->Scale(2);
    sphere1->showWireframe = true;
    sphere1->Translate({-3,0,0});
    cyl->Translate({3,0,0});
    cyl->Scale(0.12f);
    cyl->showWireframe = true;
    cube->showWireframe = true;
    camera->Translate(20, Axis::Z);
    auto mesh = cube->GetMeshList();
    Eigen::VectorXi EMAP;
    Eigen::MatrixXi F,E,EF,EI;
    Eigen::VectorXi EQ;
  // If an edge were collapsed, we'd collapse it to these points:
    Eigen::MatrixXd V, C;
    int num_collapsed;

  // Function to reset original mesh and data structures
    V = mesh[0]->GetVertices();
    F = mesh[0]->GetFaces();
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
    //cube->Rotate(0.01f, Axis::All);
}
