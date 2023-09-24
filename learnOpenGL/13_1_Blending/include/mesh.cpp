#include "mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;

  this->setupMesh();
}

void Mesh::Draw(Shader shader) {
  // 绑定合适的纹理
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  unsigned int normalNr = 1;
  unsigned int heightNr = 1;

  for(unsigned int i=0;i < this->textures.size();i++){
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = this->textures[i].type;
    if (name=="texture_diffuse") {
      number=std::to_string(diffuseNr++);
    }else if (name=="texture_specular"){
      number=std::to_string(specularNr++);
    } else if (name=="texture_normal"){
      number=std::to_string(normalNr++);
    } else if (name=="texture_height"){
      number=std::to_string(heightNr++);
    }
    glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
    glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
  }

  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, static_cast<int>(this->indices.size()), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  // 取消 texture 的绑定
  glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
  // create buffers/arrays
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(this->VAO);
  // load data into vertex buffers
  // vertex buffer object
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
  // element buffer object
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

  // set the vertex attribute pointers
  // vertex position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, TexCoords));
  // vertex tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Tangent));
  // vertex bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Bitangent));
  // iDs
  glEnableVertexAttribArray(5);
  // 设置整数类型的顶点属性数据
  glVertexAttribIPointer(5,4,GL_INT, sizeof(Vertex),(void*) offsetof(Vertex, m_BoneIDs));
  //weight
  glEnableVertexAttribArray(6);
  glVertexAttribIPointer(6,4,GL_INT, sizeof (Vertex), (void*) offsetof(Vertex, m_Weights));

  glBindVertexArray(0);
}
