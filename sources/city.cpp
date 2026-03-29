#include "city.h"
#include <iostream>

// helper: draws a model with transform 
static void drawModelTransformed(
    Model& model,
    Shader& shader,
    glm::vec3 pos,
    glm::vec3 scale,
    float rotYdeg = 0.0f)
{
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, pos);
    if (rotYdeg != 0.0f)
        m = glm::rotate(m, glm::radians(rotYdeg), glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::scale(m, scale);
    shader.setMat4("model", m);
    
    bool hasTex = (model.textureID != 0);
    shader.setBool("hasTexture", hasTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, hasTex ? model.textureID : 0);
    shader.setInt("tex0", 2);
  
    glBindVertexArray(model.VAO);
    glDrawArrays(GL_TRIANGLES, 0, model.indexCount);
    glBindVertexArray(0);
}
static bool isRoadTile(int tile)
{
    return tile == STR_H ||
        tile == STR_V ||
        tile == CURVE_NE ||
        tile == CURVE_NW ||
        tile == CURVE_SE ||
        tile == CURVE_SW ||
        tile == ROUNDABOUT ||
        tile == ST_H ||
        tile == ST_V ||
        tile == CROSS_INTERSECTION;
}

void renderCityGrid(
    Shader& shader,
    CityModels& m,
    float        bottomY,
    float        offsetX,
    float        offsetZ)
{
    const float TILE = 1.0f;
    const float startX = offsetX;
    const float startZ = offsetZ;



    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int baseTile = cityMap[row][col];
            int objTile = objectsMap[row][col];


            float wx = startX + col * TILE;
            float wz = startZ + row * TILE;
            glm::vec3 pos(wx, bottomY, wz);
            glm::vec3 pos01(wx, bottomY + 0.01f, wz);

            switch (baseTile) {
            case STR_H:
                drawModelTransformed(m.roadStraight, shader, pos, glm::vec3(1.0f), 0.0f);
                drawModelTransformed(m.roadStraightBarrier, shader, pos, glm::vec3(1.0f), 0.0f);
                break;
            case STR_V:
                drawModelTransformed(m.roadStraight, shader, pos, glm::vec3(1.0f), 90.0f);
                drawModelTransformed(m.roadStraightBarrier, shader, pos, glm::vec3(1.0f), 90.0f);
                break;
            case CURVE_SE:
                drawModelTransformed(m.roadBend, shader, pos, glm::vec3(1.0f), 180.0f);
                drawModelTransformed(m.roadBendBarrier, shader, pos, glm::vec3(1.0f), 180.0f);
                break;
            case CURVE_SW:
                drawModelTransformed(m.roadBend, shader, pos, glm::vec3(1.0f), 270.0f);
                drawModelTransformed(m.roadBendBarrier, shader, pos, glm::vec3(1.0f), 270.0f);
                break;
            case CURVE_NW:
                drawModelTransformed(m.roadBend, shader, pos, glm::vec3(1.0f), 0.0f);
                drawModelTransformed(m.roadBendBarrier, shader, pos, glm::vec3(1.0f), 0.0f);
                break;
            case CURVE_NE:
                drawModelTransformed(m.roadBend, shader, pos, glm::vec3(1.0f), 90.0f);
                drawModelTransformed(m.roadBendBarrier, shader, pos, glm::vec3(1.0f), 90.0f);
                break;
            case ROUNDABOUT:
                drawModelTransformed(m.roadRoundabout, shader, pos, glm::vec3(1.0f), 0.0f);
                drawModelTransformed(m.roundaboutBarrier, shader, pos, glm::vec3(1.0f), 0.0f);
                break;
            case HOUSE_S:
                drawModelTransformed(m.house, shader, pos01, glm::vec3(0.7f), 0.0f);
                break;
            case HOUSE_L:
                drawModelTransformed(m.house, shader, pos01, glm::vec3(1.2f), 0.0f);
                break;
            case LIGHT_TILE:
            {
                float lightRot = 0.0f;

                float lampOffsetX = 0.08f;
                float lampOffsetZ = 0.09f;
                float lampX = cityOffsetX + col * 1.0f + lampOffsetX;
                float lampZ = cityOffsetZ + row * 1.0f + lampOffsetZ;
                drawModelTransformed(m.lightSquare,
                    shader,
                    glm::vec3(lampX, bottomY, lampZ),
                    glm::vec3(1.5f),
                    lightRot);
                break;
            }
            case ST_H:
                drawModelTransformed(m.roadStraight, shader, pos, glm::vec3(1.0f), 0.0f);
                break;
            case ST_V:
                drawModelTransformed(m.roadStraight, shader, pos, glm::vec3(1.0f), 90.0f);
                break;
            case CROSS_INTERSECTION:
            {
                bool up = (row > 0) && isRoadTile(cityMap[row - 1][col]);
                bool down = (row < MAP_ROWS - 1) && isRoadTile(cityMap[row + 1][col]);
                bool left = (col > 0) && isRoadTile(cityMap[row][col - 1]);
                bool right = (col < MAP_COLS - 1) && isRoadTile(cityMap[row][col + 1]);

                float rot = 0.0f;

                if (up && !down)
                    rot = 180.0f;
                else if (down && !up)
                    rot = 0.0f;

                else if (up && down && right && !left)
                    rot = 90.0f;

                else if (up && down && left && !right)
                    rot = 270.0f;

                drawModelTransformed(m.crossIntersection, shader, pos, glm::vec3(1.0f), rot);
                drawModelTransformed(m.crossIntersectionBarrier, shader, pos, glm::vec3(1.0f), rot);
                break;
            }
            case CROSS_ROAD:
                drawModelTransformed(m.crossRoad, shader, pos, glm::vec3(1.0f), 0.0f);
                break;
            default:
                break;

            }
            switch (objTile) {
            case CONE_TILE:
            {
                float spacing = 0.25f;

                drawModelTransformed(m.constructionCone, shader, pos + glm::vec3(-spacing, 0, 0), glm::vec3(2.0f), 0.0f);
                drawModelTransformed(m.constructionCone, shader, pos + glm::vec3(0, 0, 0), glm::vec3(2.0f), 0.0f);
                drawModelTransformed(m.constructionCone, shader, pos + glm::vec3(spacing, 0, 0), glm::vec3(2.0f), 0.0f);

                break;
            }
            case CONSTRUCTION_L:
            {
                glm::vec3 objPos = pos + glm::vec3(-0.23f, 0.02f, 0.2f);

                drawModelTransformed(
                    m.constructionLight,
                    shader,
                    objPos,
                    glm::vec3(2.5f),
                    0.0f
                );
                break;
            }
            case SIGN:
            {
                glm::vec3 objPos = pos + glm::vec3(0.0f, 0.02f, 0.0f);
                drawModelTransformed(
                    m.sign,
                    shader,
                    objPos,
                    glm::vec3(1.0f),
                    180.0f
                );
                break;

            }
            }
        }
    }
}