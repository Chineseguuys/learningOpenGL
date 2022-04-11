#include "Renderer.h"
#include "ErrorCheck.h"

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
    shader.Bind();
    va.Bind();
    ib.Bind();
    // 目前这里只绘制三角形，后面再做进一步的完善
    GLCALL( glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr) );
}

void Renderer::Clear() const {
    GLCALL( glClear(GL_COLOR_BUFFER_BIT) );
}