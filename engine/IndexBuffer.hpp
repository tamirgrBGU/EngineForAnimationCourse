#pragma once


namespace cg3d
{

class IndexBuffer
{
    unsigned int m_RendererID = 0;
    unsigned int m_Count = 0;

public:
    IndexBuffer(const unsigned int *data, unsigned int count);
    IndexBuffer();
    void Init(const unsigned int *data, unsigned int count);
    IndexBuffer(const IndexBuffer &ib);
    ~IndexBuffer();
    void Bind() const;
    static void Unbind() ;
    inline unsigned int GetCount() const { return m_Count; }
    inline unsigned int GetRender() const { return m_RendererID; }
    static void ChangeData(const void *data, unsigned int count);
};

} // namespace cg3d
