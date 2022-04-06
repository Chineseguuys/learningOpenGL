#include "Texture.h"
#include "ErrorCheck.h"
#include "vendor/stb_image/stb_image.h"

Texture::Texture(const std::string& path)
    :
    m_FilePath(path),
     m_LocalBuffer(nullptr),
     m_Width(0),
     m_Height(0),
     m_BPP(0)
{
    // OpenGL 希望我们的纹理像素从左下角开始，即左下角的坐标为 (0,0)
    // 但是我们的图像是从左上角开始的，所以，为了和 OpenGL 的坐标相对应上，那么
    // 图片就需要对上下进行翻转。对于这里的例子，我们使用了 png 图片，那么我们需要对
    // 对我们的图片进行上下的翻转
    stbi_set_flip_vertically_on_load(1);
    // 4 表示图片的通道数，4的意思就是 RGBA
    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);
#ifdef DEBUG
    printf("[%s:L%d]load image for size [%dx%d]\n", __FUNCTION__, __LINE__, m_Width, m_Height);
#endif /* DEBUG */

    GLCALL( glGenTextures(1, &m_TextureID) );
    GLCALL( glBindTexture(GL_TEXTURE_2D, m_TextureID) );

    // 需要指定这四个纹理的参数
    GLCALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
    GLCALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
    GLCALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
    GLCALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );

    GLCALL( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer) );
    GLCALL( glBindTexture(GL_TEXTURE_2D, 0) );

    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}



Texture::~Texture() {
    GLCALL( glDeleteTextures(1, &m_TextureID) );
}

void Texture::Bind(unsigned int slot /* = 0*/) const {
    GLCALL( glActiveTexture(GL_TEXTURE0 + slot) );
    GLCALL( glBindTexture(GL_TEXTURE_2D, m_TextureID) );
}

void Texture::UnBind() const {
    GLCALL( glBindTexture(GL_TEXTURE_2D, 0) );
}