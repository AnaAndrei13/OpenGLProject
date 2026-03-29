#include "model.h"
//#define TINYOBJLOADER_IMPLEMENTATION
//#include "tiny_obj_loader.h"
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <filesystem>   


std::string getDirectory(const std::string& filepath)
{
    size_t pos = filepath.find_last_of("/\\");
    if (pos == std::string::npos)
        return "";
    return filepath.substr(0, pos + 1);
}

static void processNode(aiNode* node, const aiScene* scene,
    std::vector<float>& vertices,
    Model& model,
    const std::string& path,
    glm::mat4 parentTransform)
{
    aiMatrix4x4 aiMat = node->mTransformation;
    glm::mat4 nodeTransform = glm::mat4(
        aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
        aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
        aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
        aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
    );
    glm::mat4 globalTransform = parentTransform * nodeTransform;
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(globalTransform)));

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        aiColor3D baseColor(1.0f, 1.0f, 1.0f);
        if (material)
        {
            if (material->Get(AI_MATKEY_BASE_COLOR, baseColor) != aiReturn_SUCCESS)
                material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
        }

        if (model.textureID == 0 && material)
            model.textureID = loadMaterialTexture(scene, material, path);

        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            aiFace& face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                unsigned int idx = face.mIndices[j];

                // POSITION
                glm::vec4 pos = globalTransform * glm::vec4(
                    mesh->mVertices[idx].x,
                    mesh->mVertices[idx].y,
                    mesh->mVertices[idx].z, 1.0f);
                vertices.push_back(pos.x);
                vertices.push_back(pos.y);
                vertices.push_back(pos.z);

                // TEXCOORD
                if (mesh->HasTextureCoords(0)) {
                    vertices.push_back(mesh->mTextureCoords[0][idx].x);
                    vertices.push_back(mesh->mTextureCoords[0][idx].y);
                }
                else {
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }

                // COLOR
                vertices.push_back(baseColor.r);
                vertices.push_back(baseColor.g);
                vertices.push_back(baseColor.b);

                // NORMAL
                if (mesh->HasNormals()) {
                    glm::vec3 n = normalMatrix * glm::vec3(
                        mesh->mNormals[idx].x,
                        mesh->mNormals[idx].y,
                        mesh->mNormals[idx].z);
                    vertices.push_back(n.x);
                    vertices.push_back(n.y);
                    vertices.push_back(n.z);
                }
                else {
                    vertices.push_back(0.0f);
                    vertices.push_back(1.0f);
                    vertices.push_back(0.0f);
                }
            }
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene, vertices, model, path, globalTransform);
}

Model loadGLB(const std::string& path)
{
    Model model{};
    model.textureID = 0;
    model.indexCount = 0;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenSmoothNormals
    );

    if (!scene || !scene->mRootNode || scene->mNumMeshes == 0)
    {
        std::cerr << "Failed to load GLB: " << path << std::endl;
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return model;
    }

    std::vector<float> vertices;
    processNode(scene->mRootNode, scene, vertices, model, path, glm::mat4(1.0f));

    glGenVertexArrays(1, &model.VAO);
    glGenBuffers(1, &model.VBO);

    glBindVertexArray(model.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, model.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    int stride = 11 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    model.indexCount = static_cast<unsigned int>(vertices.size() / 11);

    std::cout << "Loaded GLB: " << path
        << " | vertices=" << model.indexCount
        << " | textureID=" << model.textureID
        << std::endl;

    return model;
}

void drawModel(Model& model, Shader& shader, glm::vec3 position, glm::vec3 scale, float yaw)
{
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, position);
    m = glm::rotate(m, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::scale(m, scale);

    shader.setMat4("model", m);
    shader.setBool("isModel", true);
    bool hasTex = model.textureID != 0;
    shader.setBool("hasTexture", hasTex);

    if (hasTex)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, model.textureID);
        shader.setInt("tex0", 2); // uniform sampler2D tex0
    }

    glBindVertexArray(model.VAO);
    glDrawArrays(GL_TRIANGLES, 0, model.indexCount);
    glBindVertexArray(0);

    shader.setBool("isModel", false);
}

GLuint loadTexture(const std::string& fullPath)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Schimbă REPEAT cu CLAMP
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int w, h, channels;
    
    unsigned char* data = stbi_load(fullPath.c_str(), &w, &h, &channels, 0);

    if (!data) {
        std::cerr << "FAILED: " << fullPath << " | reason: " << stbi_failure_reason() << std::endl;
        glDeleteTextures(1, &tex);
        return 0;
    }

    std::cout << "OK: " << fullPath << " | " << w << "x" << h << " ch=" << channels
        << " glID=" << tex << std::endl;
  

    GLenum format, internalFormat;

    if (channels == 1) {
        format = GL_RED;
        internalFormat = GL_RED;
    }
    else if (channels == 3) {
        format = GL_RGB;
        internalFormat = GL_RGB;
    }
    else if (channels == 4) {
        format = GL_RGBA;
        internalFormat = GL_RGBA;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(data);
    return tex;
}

GLuint loadTextureFromMemory(const unsigned char* buffer, int len)
{
    int w, h, channels;
    unsigned char* data = stbi_load_from_memory(buffer, len, &w, &h, &channels, 0);

    if (!data) {
        std::cerr << "FAILED embedded texture | reason: " << stbi_failure_reason() << std::endl;
        return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGB;
    GLenum internalFormat = GL_RGB;

    if (channels == 1) {
        format = GL_RED;
        internalFormat = GL_RED;
    }
    else if (channels == 3) {
        format = GL_RGB;
        internalFormat = GL_RGB;
    }
    else if (channels == 4) {
        format = GL_RGBA;
        internalFormat = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return tex;
}



GLuint loadMaterialTexture(const aiScene* scene, aiMaterial* material, const std::string& modelPath)
{
    if (!material) return 0;

    aiString texPath;

    // 1. glTF / PBR base color
    if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
    {
        if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath) == aiReturn_SUCCESS)
        {
            const aiTexture* embeddedTex = scene->GetEmbeddedTexture(texPath.C_Str());

            if (embeddedTex)
            {
                if (embeddedTex->mHeight == 0)
                {
                    const unsigned char* buffer =
                        reinterpret_cast<const unsigned char*>(embeddedTex->pcData);
                    int len = embeddedTex->mWidth;
                    return loadTextureFromMemory(buffer, len);
                }
            }
            else
            {
                std::string directory = getDirectory(modelPath);
                return loadTexture(directory + texPath.C_Str());
            }
        }
    }

    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == aiReturn_SUCCESS)
        {
            const aiTexture* embeddedTex = scene->GetEmbeddedTexture(texPath.C_Str());

            if (embeddedTex)
            {
                if (embeddedTex->mHeight == 0)
                {
                    const unsigned char* buffer =
                        reinterpret_cast<const unsigned char*>(embeddedTex->pcData);
                    int len = embeddedTex->mWidth;
                    return loadTextureFromMemory(buffer, len);
                }
            }
            else
            {
                std::string directory = getDirectory(modelPath);
                return loadTexture(directory + texPath.C_Str());
            }
        }
    }

    return 0;
}