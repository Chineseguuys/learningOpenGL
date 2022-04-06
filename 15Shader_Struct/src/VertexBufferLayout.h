#ifndef __VERTEX_BUFFER_LAYOUT__
#define __VERTEX_BUFFER_LAYOUT__

#include <vector>
#include "ErrorCheck.h"

struct VertexBufferElement
{
    int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch(type) {
            case GL_FLOAT:      return 4;
            case GL_UNSIGNED_INT:   return 4;
            case GL_UNSIGNED_BYTE:  return 1;
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
    VertexBufferLayout();

    template <typename T>
    void Push(unsigned int count);
    unsigned int GetStride() const;
    const std::vector<VertexBufferElement>& GetElements() const;
};


template<>
void VertexBufferLayout::Push<float>(unsigned int count);

template<>
void VertexBufferLayout::Push<unsigned int>(unsigned int count);

template<>
void VertexBufferLayout::Push<unsigned char>(unsigned int count);

#endif