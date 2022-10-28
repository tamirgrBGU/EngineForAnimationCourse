#include "gl.h"
#include "VertexBuffer.h"


namespace cg3d
{

VertexBuffer::VertexBuffer() = default;

VertexBuffer::VertexBuffer(const void *data, unsigned int size, bool dynamic)
{
    isDynamic = dynamic;
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    if (isDynamic)
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
    else
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}


void VertexBuffer::Init(const void *data, unsigned int size, bool dynamic)
{
    isDynamic = dynamic;
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    if (data) {
        if (isDynamic)
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
        else
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }
}

VertexBuffer::VertexBuffer(const VertexBuffer &vb)
{
    int size;
    isDynamic = vb.isDynamic;
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_COPY_READ_BUFFER, vb.m_RendererID);
    glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

    glBindBuffer(GL_COPY_WRITE_BUFFER, m_RendererID);
    if (isDynamic)
        glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
    else
        glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_STATIC_COPY);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::copy() const
{
    int size;
    glBindBuffer(GL_COPY_READ_BUFFER, m_RendererID);
    glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::ChangeData(const void *data, unsigned int size) const
{
    if (isDynamic) {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
    }
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace cg3d
