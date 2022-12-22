#include "BasicScene.h"
#include <read_triangle_mesh.h>
#include <utility>
#include "ObjLoader.h"
#include "IglMeshLoader.h"
#include "igl/read_triangle_mesh.cpp"
#include "igl/edge_flaps.h"
#include "BoundingBox.h"
#include "UnalignedBox.h"
#include <limits>

// #include "AutoMorphingModel.h"

using namespace cg3d;

Eigen::AlignedBox<double, 3> getAlignedBox(std::shared_ptr<cg3d::Model> model) {
    Eigen::Vector3d minCorner(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
    Eigen::Vector3d maxCorner(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
    for(auto meshList : model->GetMeshList()) {
        for(auto meshData: meshList->data) {
            auto vertices = meshData.vertices;
            for(int i=0; i<vertices.rows(); i++) {
                minCorner(0) = std::min(minCorner(0), vertices(i, 0));
                minCorner(1) = std::min(minCorner(1), vertices(i, 1));
                minCorner(2) = std::min(minCorner(2), vertices(i, 2));

                maxCorner(0) = std::max(maxCorner(0), vertices(i, 0));
                maxCorner(1) = std::max(maxCorner(1), vertices(i, 1));
                maxCorner(2) = std::max(maxCorner(2), vertices(i, 2));
            }
        }
    }

    Eigen::AlignedBox<double, 3> box(minCorner, maxCorner);
    return box;
}

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
    camel->GetMeshList()[0]->data[0].vertices;
    sphere1->Scale(2);
    sphere1->showWireframe = true;
    sphere1->Translate({-3,0,0});
    camel->Translate({3, 0, 0});
    //camel->Rotate(30, cg3d::Movable::Axis::Y);
    camel->Scale(0.12f);
    camel->showWireframe = true;
    camera->Translate(20, Axis::Z);
    root->AddChild(sphere1);
    root->AddChild(camel);



    leftTree.init(sphere1->GetMesh()->data[0].vertices, sphere1->GetMesh()->data[0].faces);
    rightTree.init(camel->GetMesh()->data[0].vertices, camel->GetMesh()->data[0].faces);
    int x = 2;
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

    CheckAndHandleCollusion();

}


Eigen::Matrix3Xd alignedBoxToMatrix(Eigen::AlignedBox<double, 3> box) {
    Eigen::Matrix<double, 3, 8> mat;

    auto v0 = box.corner(box.BottomLeftFloor);
    auto v1 = box.corner(box.BottomRightFloor);
    auto v2 = box.corner(box.TopLeftFloor);
    auto v3 = box.corner(box.TopRightFloor);
    auto v4 = box.corner(box.BottomLeftCeil);
    auto v5 = box.corner(box.BottomRightCeil);
    auto v6 = box.corner(box.TopLeftCeil);
    auto v7 = box.corner(box.TopRightCeil);

    mat <<  v0.x(), v1.x(), v2.x(), v3.x(), v4.x(), v5.x(), v6.x(), v7.x(),
            v0.y(), v1.y(), v2.y(), v3.y(), v4.y(), v5.y(), v6.y(), v7.y(),
            v0.z(), v1.z(), v2.z(), v3.z(), v4.z(), v5.z(), v6.z(), v7.z();


    return mat;
}

UnalignedBox matrixToUnalignedBox(Eigen::Matrix3Xd matrix) {
    std::vector<Eigen::Vector3d> vertices;
    for(int i=0; i<matrix.cols(); i++) {
        vertices.emplace_back(matrix.col(i));
    }

    return UnalignedBox::BuildBoxFromVertices(vertices);
}

std::vector<igl::AABB<Eigen::MatrixXd, 3>> goDown(igl::AABB<Eigen::MatrixXd, 3> tree) {
    if(tree.is_leaf()) {
        return {tree};
    }
    if(tree.m_left == nullptr) {
        return {*tree.m_right};
    }
    if(tree.m_right == nullptr) {
        return {*tree.m_left};
    }

    return {*tree.m_left, *tree.m_right};
}

UnalignedBox fromBoxAndModel(Eigen::AlignedBox<double, 3> box, std::shared_ptr<cg3d::Model> m) {
    std::vector<Eigen::Vector3d> vertices;
    Eigen::Matrix4d transform = (m->GetTransform()).cast<double>();
    auto matrix = alignedBoxToMatrix(box);
    for(int i=0; i<matrix.cols(); i++) {
        Eigen::Vector4d vertex;
        vertex << matrix(0, i),  matrix(1, i), matrix(2, i), 1;
        Eigen::Vector4d fixed4D = transform * vertex;
        Eigen::Vector3d fixed3D;
        fixed3D << fixed4D.x(), fixed4D.y(), fixed4D.z();
        vertices.push_back(fixed3D);
    }
    return UnalignedBox::BuildBoxFromVertices(vertices);
}

bool doNodesCollide(igl::AABB<Eigen::MatrixXd, 3> tree1, std::shared_ptr<cg3d::Model> m1,
                    igl::AABB<Eigen::MatrixXd, 3> tree2, std::shared_ptr<cg3d::Model> m2) {

    auto box1 = fromBoxAndModel(tree1.m_box, m1);
    auto box2 = fromBoxAndModel(tree2.m_box, m2);

    return UnalignedBox::CheckCollusion(box1, box2);
}

bool leavesOnly(std::vector<igl::AABB<Eigen::MatrixXd, 3>> trees)  {
    for(auto node : trees) {
        if(!node.is_leaf()) {
            return false;
        }
    }
    return true;
}

bool doCollide(igl::AABB<Eigen::MatrixXd, 3> tree1, std::shared_ptr<cg3d::Model> m1,
                    igl::AABB<Eigen::MatrixXd, 3> tree2, std::shared_ptr<cg3d::Model> m2) {
    std::vector<igl::AABB<Eigen::MatrixXd, 3>> nodes1 = {tree1};
    std::vector<igl::AABB<Eigen::MatrixXd, 3>> nodes2 = {tree2};

    while(!nodes1.empty() && !leavesOnly(nodes1)) {
        std::vector<igl::AABB<Eigen::MatrixXd, 3>> newNodes1;
        std::vector<igl::AABB<Eigen::MatrixXd, 3>> newNodes2;

        for (auto node1: nodes1) {
            if (doNodesCollide(node1, m1, tree2, m2)) {
                newNodes1.push_back(node1);
            }
        }
        nodes1.clear();
        for (auto n1: newNodes1) {
            auto down = goDown(n1);
            for (auto n: down) {
                nodes1.push_back(n);
            }
        }
    }

    return !nodes1.empty();
}

void BasicScene::CheckAndHandleCollusion() {
    if(doCollide(leftTree, sphere1, rightTree, camel)) {
        HandleCollusion(sphere1, camel);
    }
}

void BasicScene::HandleCollusion(std::shared_ptr<cg3d::Model> model1, std::shared_ptr<cg3d::Model> model2) {
    std::cout << "Collusion detected between " << model1->name << " and " << model2->name << "!" << std::endl;
}
