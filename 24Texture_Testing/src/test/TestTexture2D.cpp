#include "TestTexture2D.h"
#include "imgui.h"
#include "ErrorCheck.h"

namespace test {

TestTexture2D::TestTexture2D() 
    : m_TranslationA(100, 100, 0),
      m_TranslationB(300, 100, 0),
      m_Proj(glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f)),
      m_View(glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0)))
{
    float position[] = {
        100.0f,  100.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        400.0f,  100.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        400.0f,  400.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        100.0f,  400.0f, 0.0f, 1.0f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2 ,   // 这些顶点构成了第一个三角形 这个三角形位于右下角 两个三角形共用了两个顶点
        2, 3, 0     // 这些顶点构成了第二个三角形 这个三角形位于左上角
    };

    GLCALL( glEnable(GL_BLEND) );
    GLCALL( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
    //GLCALL( glBlendFunc(GL_ONE, GL_ZERO) );   // 这是 OpenGL 默认的混合的方式
    GLCALL( glBlendEquation(GL_FUNC_ADD) );

    // 每一个顶点具有四个浮点数，其中的两个是顶点坐标，另外的两个是纹理坐标
    //VertexBuffer vb(position , 4 * 6 * sizeof(float) );
    m_VertexBuffer = std::make_unique<VertexBuffer>( position , 4 * 6 * sizeof(float) );

    m_VAO = std::make_unique<VertexArray>();
    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

    //VertexBufferLayout layout;
    m_VertexBufferLayout = std::make_unique<VertexBufferLayout>();
    m_VertexBufferLayout->Push<float>(4);
    m_VertexBufferLayout->Push<float>(2);
    // 前面的两个是顶点坐标，另外两个是纹理坐标
    //layout.Push<float>(4);
    //layout.Push<float>(2);

    m_VAO->AddBuffer(*m_VertexBuffer, *m_VertexBufferLayout);
    //va.AddBuffer(*vb, layout);

    m_Shader =  std::make_unique<Shader>("./res/shaders/Basic.shader");
    m_Shader->Bind();

    // 纹理
    m_Texture = std::make_unique<Texture>("./res/textures/Vulcan-1-3.png");

    m_Shader->SetUniform1i("u_Texture", 0);

    //shader.SetUniformMat4f("u_MVP", mvp);
    GLCALL( glBindVertexArray(0) );
}

TestTexture2D::~TestTexture2D() {}

void TestTexture2D::OnImGuiRender() {
    ImGui::SliderFloat3("TranslationA", &m_TranslationA.x, 0.0f, 640.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat3("TranslationB", &m_TranslationB.x, 0.0f, 640.0f);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void TestTexture2D::OnRender() {
    GLCALL( glClearColor(0.0f, 0.0f, 0.0f, 1.0f) );
    GLCALL( glClear(GL_COLOR_BUFFER_BIT) );

    Renderer renderer;

    m_Texture->Bind(0); // 绑定在第一个插槽上面

    {
        // 模型变换 表示对当前的需要绘制的对象进行变换
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
        // 注意矩阵乘法的方式从右边向左边
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        // Draw() 函数当中包含了 VertexArray 以及 IndexBuffer 和 Shader 的绑定 Bind() 操作
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        // Draw() 函数当中包含了 VertexArray 以及 IndexBuffer 和 Shader 的绑定 Bind() 操作
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }
}


void TestTexture2D::OnUpdate(float deltaTime) {}

} /* namespace test */