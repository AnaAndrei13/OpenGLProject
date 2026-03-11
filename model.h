#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

struct Model {
    unsigned int VAO, VBO;
    int vertexCount;
    glm::vec3 color;
};

Model loadOBJ(const char* path);
void drawModel(Model& model, Shader& shader, glm::vec3 position, glm::vec3 scale);