#ifndef __MATERIAL__
#define __MATERIAL__

#include <glm/glm.hpp>
#define SHININESS_CONSTV  128.0f

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Material emerald {
    .ambient = glm::vec3(0.0215, 0.1745, 0.0215),
    .diffuse = glm::vec3(0.07568, 0.61424, 0.07568),
    .specular = glm::vec3(0.633, 0.727811, 0.633),
    .shininess = 0.6 * SHININESS_CONSTV
};

struct Material jade {
    .ambient = glm::vec3(0.135,	0.2225,	0.1575),
    .diffuse = glm::vec3(0.54, 0.89, 0.63),
    .specular = glm::vec3(0.316228, 0.316228, 0.316228),
    .shininess = 0.1f * SHININESS_CONSTV
};

struct Material obsidian {
    .ambient = glm::vec3(0.05375,	0.05,	0.06625),
    .diffuse = glm::vec3(0.18275,	0.17,	0.22525),
    .specular = glm::vec3(0.332741,	0.328634,	0.346435),
    .shininess = 0.3 * SHININESS_CONSTV
};

struct Material pearl {
    .ambient = glm::vec3(0.25,	0.20725,	0.20725),
    .diffuse = glm::vec3(1,	0.829,	0.829),
    .specular = glm::vec3(0.296648,	0.296648,	0.296648),
    .shininess = 0.088 * SHININESS_CONSTV
};

struct Material rupy {
    .ambient = glm::vec3(0.1745,	0.01175,	0.01175),
    .diffuse = glm::vec3(0.61424,	0.04136,	0.04136),
    .specular = glm::vec3(0.727811,	0.626959,	0.626959),
    .shininess = 0.6 * SHININESS_CONSTV
};

#endif /* __MATERIAL__ */