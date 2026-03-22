#include "model.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

Model loadOBJ(const char* path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path, "./");
   
    if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
    if (!err.empty())  std::cerr << "ERR: " << err << std::endl;
    if (!ret) { std::cerr << "Failed to load: " << path << std::endl; return {}; }
    std::vector<float> vertices;
 
    for (auto& shape : shapes) {
        for (auto& idx : shape.mesh.indices) {
            // position(location 0)
            vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
           
            // texture coordinates (location 1)
            if (idx.texcoord_index >= 0) {
                vertices.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                vertices.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
            }
            else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
            // vertex color (location 2)
            if (!attrib.colors.empty()) {
                vertices.push_back(attrib.colors[3 * idx.vertex_index + 0]);
                vertices.push_back(attrib.colors[3 * idx.vertex_index + 1]);
                vertices.push_back(attrib.colors[3 * idx.vertex_index + 2]);
            }
            else {
                vertices.push_back(1.0f);
                vertices.push_back(1.0f);
                vertices.push_back(1.0f);
            }
            // normal (location 3)
            if (idx.normal_index >= 0) {
                vertices.push_back(attrib.normals[3 * idx.normal_index + 0]);
                vertices.push_back(attrib.normals[3 * idx.normal_index + 1]);
                vertices.push_back(attrib.normals[3 * idx.normal_index + 2]);
            }
            else {
                // default normal pointing up if missing
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
            }
         
        }
    }
    if (vertices.size() > 0) {
        std::cout << path << " first UV: " << vertices[3] << ", " << vertices[4] << std::endl;
    }

    Model model;
    model.vertexCount = vertices.size() / 11;

    glGenVertexArrays(1, &model.VAO);
    glGenBuffers(1, &model.VBO);
    glBindVertexArray(model.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, model.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //vertex color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    model.color = glm::vec3(1.0f, 1.0f, 1.0f);
    return model;
}

void drawModel(Model& model, Shader& shader, glm::vec3 position, glm::vec3 scale)
{
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, position);
    m = glm::scale(m, scale);
    shader.setMat4("model", m);

    glBindVertexArray(model.VAO);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindVertexArray(0);
}