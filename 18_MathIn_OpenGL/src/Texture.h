#ifndef __TEXTURE_OPENGL__
#define __TEXTURE_OPENGL__


#include "Renderer.h"

class Texture
{
private:
    // 纹理的 ID
    unsigned int m_TextureID;
    // 用来绘制纹理的图片的路径
    std::string m_FilePath;
    // 本地的图片数据的缓冲
    unsigned char* m_LocalBuffer;
    // 图片相关的结构信息，长度，宽度以及图像深度
    int m_Width, m_Height, m_BPP;
public:
    Texture(const std::string& path);
    ~Texture();

    void Bind(unsigned int slot /*= 0*/) const;
    void UnBind() const;

    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }
    inline int GetBPP() const { return m_BPP; }
};

#endif