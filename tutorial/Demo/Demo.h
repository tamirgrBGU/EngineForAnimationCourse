#pragma once

#pragma warning(push, 3)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)

#include "Scene.h"

#pragma warning(pop)

using Eigen::Matrix4f;

class Demo : public Scene
{
private:
    bool isActive = false;

public:
    shared_ptr<Camera> camera1;
    shared_ptr<Camera> camera2;
    shared_ptr<Camera> camera3;
    shared_ptr<Camera> currentCamera;

    explicit Demo(const string &name) : Scene(name) {}
    void Init();
    void Update(const Matrix4f& proj, const Matrix4f& view, const Matrix4f& model, const shared_ptr<const Program>& program) override;
    inline bool IsActive() const { return isActive; };
    inline void SetActive(bool _isActive = true) { isActive = _isActive; }
};

