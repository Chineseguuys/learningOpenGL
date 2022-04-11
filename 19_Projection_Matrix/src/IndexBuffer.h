#ifndef __INDEXBUFFER_OPENGL__
#define __INDEXBUFFER_OPENGL__


class IndexBuffer
{
private:
    unsigned int m_RendererID;
    unsigned int m_Counts;
public:
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetCount() const { return m_Counts; }
};

#endif