#include "Shape.h"

#include "ViewerData.h"
#include "glad/glad.h"
#include "debug.h"

shared_ptr<Shape> Shape::Create(const string& file, const shared_ptr<const Material>& material, const shared_ptr<Movable>& parent)
{
    return Create(file, make_shared<const Mesh>(file), material, parent);
}

shared_ptr<Shape> Shape::Create(const string& name, const string& file, const shared_ptr<const Material>& material, const shared_ptr<Movable>& parent)
{
    return Create(name, make_shared<const Mesh>(file), material, parent);
}

shared_ptr<Shape> Shape::Create(const shared_ptr<const Mesh>& mesh, const shared_ptr<const Material>& material, const shared_ptr<Movable>& parent)
{
    return Create(mesh->name, mesh, material, parent);
}

shared_ptr<Shape> Shape::Create(const string& name, const shared_ptr<const Mesh>& mesh, const shared_ptr<const Material>& material, const shared_ptr<Movable>& parent)
{
    auto shape = shared_ptr<Shape>(new Shape(name, mesh, material));
    shape->SetParent(parent);
    return shape;
}

Shape::Shape(const string& name, const shared_ptr<const Mesh>& _mesh, const shared_ptr<const Material>& material)
        : Movable(name), mesh(_mesh), material(material), viewports(1) // TODO: TAL: add support for different viewports
{
    viewerData.set_mesh(mesh->V, mesh->F);
    viewerData.set_uv(mesh->V_uv);
    viewerData.set_normals(mesh->V_normals);
    viewerData.set_face_based(true);
    viewerData.line_width = 1.0f;

    viewerData.uniform_colors(Vector3d(1.0, 1.0, 1.0), Vector3d(1.0, 1.0, 1.0), Vector3d(1.0, 1.0, 1.0)); // TODO: TAL: ?

    //viewerData.compute_normals(); // TODO: TAL: does this needs to be called if V_normals is empty?

    // Elik: why?
    if (viewerData.V_uv.rows() == 0)
        viewerData.grid_texture();

    viewerData.is_visible = 1;
    viewerData.show_overlay = 0;

/*  TODO: TAL: add support for an "axis shape" (should that be a mesh, or...?)
    d->type = type; // overwrite the value MeshCopy
    if (type == Axis) {
        d->is_visible = 0;
        d->show_faces = 0;
        d->show_overlay = 0xFF;
    } */
}

void Shape::Draw(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& norm, int viewportIndex, unsigned int flags) // NOLINT(misc-no-recursion)
{
    if (Is2Render(viewportIndex)) 
    {
        auto program = material->BindProgram();
        Matrix4f model = CalculateModelTransform(view, norm);
        Update(proj, view, model, program);
        ViewerDataUpdateAndBindMesh(program);
        material->BindProgram(); // call BindProgram() again to rebind the textures because IGL's bind_mesh messes them up
        viewerData.meshgl.draw_mesh(true);

        if (showWireframe) { // wireframe shows only if we're not in picking mode
            program = material->BindFixedColorProgram();
            Update(proj, view, model, program);
            program->SetUniform4f("fixedColor", 0.0f, 0.0f, 0.0f, 1.0f); // black
            glLineWidth(viewerData.line_width);
            viewerData.meshgl.shader_mesh = program->GetHandle();
            viewerData.meshgl.draw_mesh(false);
        }
    }

    Movable::Draw(proj, view, norm, viewportIndex, flags);
}

void Shape::DrawForPicking(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& norm, int viewportIndex, PickingData* pickingData) // NOLINT(misc-no-recursion)
{
    if (Is2Render(viewportIndex) && isPickable) 
    {
        pickingData->id++;
        pickingData->objects.emplace_back(shared_from_this());

        auto program = material->BindFixedColorProgram();
        Matrix4f model = CalculateModelTransform(view, norm);
        Update(proj, view, model, program);

        int r = (pickingData->id & 0x000000FF) >> 0;
        int g = (pickingData->id & 0x000000FF) >> 8;
        int b = (pickingData->id & 0x000000FF) >> 16;
        program->SetUniform4f("fixedColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

        ViewerDataUpdateAndBindMesh(program);
        viewerData.meshgl.draw_mesh(true);
    }

    Movable::DrawForPicking(proj, view, norm, viewportIndex, pickingData);
}

Matrix4f Shape::CalculateModelTransform(const Matrix4f& view, const Matrix4f& norm) // helper function
{
    Matrix4f model;

    if (inverseViewTransform)
        model = view.inverse() * Tin.matrix() * Tout.matrix();
    else if (isStatic)
        model = transform; // TODO: TAL: why in static object we don't need the normal...!?
    else
        model = norm * transform;

    return model;
}

void Shape::ViewerDataUpdateAndBindMesh(shared_ptr<const Program> program) // helper function
{
    viewerData.updateGL(viewerData, viewerData.invert_normals, viewerData.meshgl);
    viewerData.dirty = igl::opengl::MeshGL::DIRTY_NONE;
    viewerData.meshgl.shader_mesh = program->GetHandle();
    viewerData.meshgl.bind_mesh();
}
