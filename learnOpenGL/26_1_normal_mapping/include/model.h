//
// Created by yanjianghan on 2023/8/22.
//

#ifndef ASSIMP_MODEL_INCLUDE_MODEL_H
#define ASSIMP_MODEL_INCLUDE_MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stbi_images.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include "string"
#include "fstream"
#include "sstream"
#include "iostream"
#include "map"
#include "vector"

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model {
public:
  // stores all the textures loaded so far, optimization to make sure textures aren't loaded more then once.
  std::vector<Texture> textures_loaded;
  std::vector<Mesh> meshes;
  std::string directory;
  bool gammaCorrection;

  Model(std::string const &path, bool gamma = false);

  void Draw(Shader &shader);
private:
  void LoadModel(std::string const &path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif //ASSIMP_MODEL_INCLUDE_MODEL_H
