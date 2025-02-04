#ifndef MESH_H
#define MESH_H

#include "shader.h"
#include <spdlog/spdlog.h>
#include <filesystem>

Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath)
{
    spdlog::debug("Shader::Shader({}, {})", vertexPath == nullptr ? "null" : vertexPath, fragmentPath == nullptr ? "null" : fragmentPath);
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (geometryPath != nullptr) {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }catch (std::ifstream::failure& e) {
        spdlog::critical("{}: ERROR:SHADER::FILE_NOT_SUCCESSFULLY_READ, what={}", __FUNCTION__, e.what());
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    spdlog::info("Vertex Shader:\n{}", vShaderCode);
    spdlog::info("Fragment Shader:\n{}", fShaderCode);
    // 编译着色器
    unsigned int vertex, fragment;
    // vertex shader
    vertex=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    unsigned  int geometry;
    if (geometryPath != nullptr) {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, nullptr);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertex);
    glAttachShader(this->ID, fragment);
    if (geometryPath != nullptr) {
        glAttachShader(this->ID, geometry);
    }
    glLinkProgram(this->ID);
    checkCompileErrors(ID, "PROGRAM");

    spdlog::debug("link program with ID {}", this->ID);
    this->mVertexPath = std::string(vertexPath);
    this->mFragmentPath = std::string(fragmentPath);
    this->mShaderName = getShaderNameFromPath(vertexPath);
    spdlog::debug("Set Shader Name: {}", this->mShaderName.c_str());
    //saveProgramBinary();

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr) {
        glDeleteShader(geometry);
    }
}

void Shader::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            //std::cout << "ERROR:SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n" << std::endl;
            spdlog::critical("{}: ERROR:SHADER_COMPILATION_ERROR of type: {}", __FUNCTION__, type);
            spdlog::critical("{}: error info log: {}", __FUNCTION__, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            //std::cout << "ERROR:PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n" << std::endl;
            spdlog::critical("{}: ERROR:PROGRAM_LINKING_ERROR of type: {}", __FUNCTION__ , type);
            spdlog::critical("{}: error info log: {}", __FUNCTION__ ,infoLog);
        }
    }
}

std::string Shader::getShaderNameFromPath(const char* path) {
    std::string result;
    result = std::filesystem::path(path).filename().string();
    spdlog::debug("From path {} we get the file name {}", path, result.c_str());
    size_t pointPos = result.find_last_of('.');
    return result.substr(0, pointPos);
}

void Shader::use() {
    glUseProgram(this->ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(this->ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(this->ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(this->ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
  glUniformMatrix3fv(glGetUniformLocation(this->ID,name.c_str()), 1, GL_FALSE,&mat[0][0]);
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(this->ID,name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

Shader::~Shader() {
    if (this->ID != 0) {
      spdlog::info("{}: delete program with ID {}", __FUNCTION__ , this->ID);
      glDeleteProgram(this->ID);
    }
}

void Shader::saveProgramBinary() {
    GLint programLen = 0;
    glGetProgramiv(this->ID, GL_PROGRAM_BINARY_LENGTH, &programLen);
    spdlog::debug("Try to save program with ID: {}, binary length is: {}", this->ID, programLen);
    if (programLen <= 0) {
        spdlog::critical("Can not contain the binary length of program {}", this->ID);
        return;
    }
    std::vector<GLubyte> binary(programLen);
    GLint formats = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
    spdlog::debug("Program with ID:{}, binary formats length is {}", this->ID, formats);
    std::vector<GLenum> binaryFormats(formats);
    glGetProgramBinary(this->ID, programLen, nullptr, binaryFormats.data(), binary.data());
    GLenum err = glGetError();
    switch (err)
    {
    case GL_INVALID_OPERATION:
        spdlog::critical("Program Not Successfully Linked!");
        return;
    case GL_INVALID_VALUE:
        spdlog::critical("Buffer Size too Small!");
        return;
    case GL_NO_ERROR:
        spdlog::debug("glGetProgramBinary successful returned!");
        break;
    default:
        spdlog::critical("Unknow error");
        break;
    }
    std::string programCacheFile = this->mShaderName + ".cache";
    std::ofstream file(programCacheFile, std::ios::binary);
    if (file) {
        file.write(reinterpret_cast<char*>(binaryFormats.data()), formats * sizeof(GLenum));
        file.write(reinterpret_cast<char*>(binary.data()), programLen);
        spdlog::info("Write program with ID: {} to file {}, with formats len: {}, binary len: {}", this->ID, programCacheFile.c_str(), formats * sizeof(GLenum), programLen);
    }
}

#endif /* MESH_H */