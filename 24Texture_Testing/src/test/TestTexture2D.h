#ifndef __TEST_TEXTURE2D_OPENGL__
#define __TEST_TEXTURE2D_OPENGL__

#include "Test.h"

// 已经提前安装在系统 include 目录当中，这里不再单独创建子工程文件夹
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// smart pointer
#include <memory>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"

namespace test {

class TestTexture2D : public Test {
    public:
        TestTexture2D();
        ~TestTexture2D();

        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;

    private:
        glm::vec3 m_TranslationA, m_TranslationB;
        glm::mat4 m_Proj, m_View;
        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<Texture> m_Texture;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexBufferLayout> m_VertexBufferLayout;
};


}   /* namespace test */

#endif /* __TEST_TEXTURE2D_OPENGL__ */