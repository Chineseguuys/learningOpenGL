#include "VertexArray.h"

VertexArray::VertexArray() {
    GLCALL( glGenVertexArrays(1, &m_RendererID) );
}

VertexArray::~VertexArray() {
    GLCALL( glDeleteVertexArrays(1, &m_RendererID) );
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
    this->Bind();
    vb.Bind();
    const std::vector<VertexBufferElement>& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); ++i) {
        const VertexBufferElement& element = elements[i];
        GLCALL( glEnableVertexAttribArray(i) );
        GLCALL( 
            glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), reinterpret_cast<void*>(offset)) 
        );
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}

void VertexArray::Bind() const{
    GLCALL( glBindVertexArray(m_RendererID) );
}

void VertexArray::UnBind() const {
    GLCALL( glBindVertexArray(0) );
}