#include <glad/glad.h>
#include <vector>
#include <cmath>

unsigned int terrainVAO, terrainVBO;
int terrainVertexCount;

void setupTerrain() {
    int gridSize = 100;
    float cellSize = 2.0f / gridSize;

    std::vector<float> vertices;

    for (int z = 0; z < gridSize; z++) {
        for (int x = 0; x < gridSize; x++) {
            float x0 = -1.0f + x * cellSize;
            float x1 = x0 + cellSize;
            float z0 = -1.0f + z * cellSize;
            float z1 = z0 + cellSize;

            auto height = [](float x, float z) {
                float dx = x - 1.0f;   
                float dz = z + 0.75f;   
                return 3.5f * exp(-(dx * dx + dz * dz) * 7.0f);
                };

            float y00 = height(x0, z0);
            float y10 = height(x1, z0);
            float y01 = height(x0, z1);
            float y11 = height(x1, z1);

            // triangle 1
            vertices.insert(vertices.end(), { x0, y00, z0,  0.0f, 0.0f });
            vertices.insert(vertices.end(), { x1, y10, z0,  1.0f, 0.0f });
            vertices.insert(vertices.end(), { x0, y01, z1,  0.0f, 1.0f });

            // triangle 2
            vertices.insert(vertices.end(), { x1, y10, z0,  1.0f, 0.0f });
            vertices.insert(vertices.end(), { x1, y11, z1,  1.0f, 1.0f });
            vertices.insert(vertices.end(), { x0, y01, z1,  0.0f, 1.0f });
        }
    }

    terrainVertexCount = vertices.size() / 5;

    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glBindVertexArray(terrainVAO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

