#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"

struct Model {
    unsigned int VAO, VBO,EBO;
    unsigned int textureID=0;
    size_t indexCount=0;
};

Model loadGLB(const std::string& path);
GLuint loadTextureFromMemory(const unsigned char* buffer, int len);
void drawModel(Model& model, Shader& shader, glm::vec3 position, glm::vec3 scale, float yaw);
GLuint loadTexture(const std::string& fullPath);
std::string getDirectory(const std::string& filepath);
GLuint loadMaterialTexture(const aiScene* scene, aiMaterial* material, const std::string& modelPath);
static void processNode(aiNode* node, const aiScene* scene,
    std::vector<float>& vertices,
    Model& model,
    const std::string& path,
    glm::mat4 parentTransform);