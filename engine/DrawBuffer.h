#pragma once

#include <Eigen/Core>


namespace cg3d
{

class DrawBuffer
{
    //NOTE: old engine
    unsigned int frameBuffer;
    unsigned int renderBuffer;
    unsigned int depthBuffer;
    unsigned int depthStencilBuffer;
    bool isStencil;

    void CreateDepthStencilBufferAttachment(int width, int height, unsigned int texId);
    void CreateColorBufferAttachment(int width, int height, unsigned int texId);

public:
    //NOTE: old engine
    enum
    {
        COLOR,
        DEPTH,
        STENCIL,
        BACK,
        FRONT,
        NONE,
    };

    DrawBuffer();
    DrawBuffer(int width, int height, unsigned int texId);
    void resize(int width, int height, unsigned int texId);
    void Bind();
    void UnBind();
    ~DrawBuffer();

    //NOTE: needed for the new one
    void clearFrameBuffers(Eigen::Vector4i viewport, Eigen::Vector4f background_color);
};

} // namespace cg3d
