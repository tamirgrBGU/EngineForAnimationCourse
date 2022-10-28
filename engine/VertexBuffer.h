#pragma once


namespace cg3d
{

class VertexBuffer
{
    unsigned int m_RendererID = 0;
    bool isDynamic = false;

public:

    VertexBuffer(const VertexBuffer& vb);
    VertexBuffer(const void* data, unsigned int size, bool dynamic = false);
    ~VertexBuffer();
    void ChangeData(const void* data, unsigned int size) const;
    void Bind() const;
    static void Unbind() ;
    void copy() const;
    void Init(const void* data, unsigned int size, bool dynamic);
    VertexBuffer();
};

} // namespace cg3d
