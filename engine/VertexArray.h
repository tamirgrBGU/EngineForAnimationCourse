#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.hpp"


namespace cg3d
{

class VertexArray
{
    unsigned int m_RendererID = 0;

public:

    VertexArray();
    ~VertexArray();
    static void AddBuffer(const VertexBuffer &vb, int attribNum, int count, int type);
    void Bind() const;
    static void Unbind() ;
};

} // namespace cg3d
