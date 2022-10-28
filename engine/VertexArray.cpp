#include "VertexArray.h"
#include "gl.h"


namespace cg3d
{

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::AddBuffer(const VertexBuffer &vb, int attribNum, int count, int type)
{
    //vb.Bind();
    glEnableVertexAttribArray(attribNum);
    glVertexAttribPointer(attribNum, count, type, GL_FALSE, 0, nullptr);
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind()
{
    glBindVertexArray(0);
}

} // namespace cg3d
