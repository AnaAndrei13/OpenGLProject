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

    glBindVertexArray(model.VAO);
    glEnableVertexAttribArray(2);
    glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);
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
            int tile = cityMap[row][col];
            if (tile == EMPTY) continue;

            float wx = startX + col * TILE;
            float wz = startZ + row * TILE;
            glm::vec3 pos(wx, bottomY, wz);
            glm::vec3 pos01(wx, bottomY + 0.01f, wz);

            switch (tile) {
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
            case T_SOUTH:
                drawModelTransformed(m.roadStraight, shader, pos, glm::vec3(1.0f), 0.0f);
                break;
            case T_NORTH:
                drawModelTransformed(m.roadStraight, shader, pos, glm::vec3(1.0f), 180.0f);
                break;
            case T_WEST:
                drawModelTransformed(m.roadStraight, shader, pos, glm::vec3(1.0f), 90.0f);
                break;
            case T_EAST:
                drawModelTransformed(m.roadStraight, shader, pos, glm::vec3(1.0f), 270.0f);
                break;
            case CROSS:
                drawModelTransformed(m.roadIntersection, shader, pos, glm::vec3(1.0f), 0.0f);
                drawModelTransformed(m.roadStraightBarrier, shader, pos, glm::vec3(1.0f), 0.0f);
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
                drawModelTransformed(m.lightSquare, shader, pos01, glm::vec3(1.5f), 0.0f);
                break;
            case CONE_TILE:
                drawModelTransformed(m.constructionCone, shader,
                    glm::vec3(wx +0.7f , bottomY + 0.01f, wz),
                    glm::vec3(2.2f), 0.0f);
                drawModelTransformed(m.constructionCone, shader,
                    glm::vec3(wx + 1.0f, bottomY + 0.01f, wz),
                    glm::vec3(2.2f), 0.0f);
                drawModelTransformed(m.constructionCone, shader,
                    glm::vec3(wx + 1.3f, bottomY + 0.01f, wz),
                    glm::vec3(2.2f), 0.0f);
                break;
             case ST_H:
                drawModelTransformed(m.roadStraight, shader, pos, glm::vec3(1.0f), 0.0f);
                break;
             case ST_V:
                drawModelTransformed(m.roadStraight, shader, pos, glm::vec3(1.0f), 90.0f);
				break;
            default:
                break;
            }
        }
    }
}