#include "IndexBuffer.h"
#include "ErrorCheck.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) 
    : m_Counts(count)
{
    // 在一些平台当中， GLuint 的长度可能和系统的 unsigned int 的大小不一致，这个时候需要格外的小心
    ASSERT( sizeof(unsigned int) == sizeof(GLuint) );

    GLCALL( glGenBuffers(1, &m_RendererID) );
    GLCALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID) );
    GLCALL( glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW) );
}


IndexBuffer::~IndexBuffer() {
    GLCALL( glDeleteBuffers(1, &m_RendererID) );
}

void IndexBuffer::Bind() const {
    GLCALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID) );
}

void IndexBuffer::Unbind() const {
    GLCALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
}