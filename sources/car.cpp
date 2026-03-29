#include "car.h"
#include "city.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

// Initialize the car at the starting position
void initCar(Car& car, float cityOffsetX, float cityOffsetZ, float bottomY)
{
    car.position = glm::vec3(cityOffsetX + 1.0f, bottomY + 0.01f, cityOffsetZ + 2.6f);
    car.yaw = 0.0f;
    car.speed = 0.0f;
}

void initDeliveryCar(Car& car, float cityOffsetX, float cityOffsetZ, float bottomY)
{
    car.position = glm::vec3(cityOffsetX + 3.0f, bottomY + 0.01f, cityOffsetZ + 2.0f);
    car.yaw = 90.0f;
    car.speed = 0.0f;
    car.targetCorner = 1;
}

void initVanCar(Car& car, float cityOffsetX, float cityOffsetZ, float bottomY)
{
    car.position = glm::vec3(cityOffsetX + 1.0f, bottomY + 0.01f, cityOffsetZ + 7.0f);
    car.yaw = 0.0f;
    car.speed = 0.0f;

}


// Update car movement based on keyboard input
void updateCar(Car& car, GLFWwindow* window, float cityOffsetX, float cityOffsetZ)
{
    glm::vec3 prevPos = car.position;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        car.speed += 0.0006f;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        car.speed -= 0.0006f;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        car.speed = 0.0f;

    if (fabs(car.speed) > 0.0001f) {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            car.yaw += 1.5f;

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            car.yaw -= 1.5f;
    }

    car.speed *= 0.98f;

    // limit
    if (car.speed > 0.03f) car.speed = 0.03f;
    if (car.speed < -0.02f) car.speed = -0.02f;

    //movement in the direction the car is facing
    car.position.x += sin(glm::radians(car.yaw)) * car.speed;
    car.position.z += cos(glm::radians(car.yaw)) * car.speed;

    // collision
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int tile = cityMap[row][col];
            int objTile = objectsMap[row][col];

            float hx = cityOffsetX + col * 1.0f;
            float hz = cityOffsetZ + row * 1.0f;

            if (tile == HOUSE_L || tile == HOUSE_S) {
                if (fabs(car.position.x - hx) < 0.7f &&
                    fabs(car.position.z - hz) < 0.7f)
                {
                    car.position = prevPos;
                    car.speed = 0.0f;
                    return;
                }
            }
            bool blocksFromObjectMap = (
                objTile == CONE_TILE ||
                objTile == CONSTRUCTION_L
                );

            if (blocksFromObjectMap) {
                if (fabs(car.position.x - hx) < 0.5f &&
                    fabs(car.position.z - hz) < 0.5f)
                {
                    car.position = prevPos;
                    car.speed = 0.0f;
                    return;
                }
            }
        }
    }
}

void renderCar(const Car& car, Model& carModel, Shader& shader)
{
    glDisable(GL_CULL_FACE);
    glm::mat4 carMatrix = glm::mat4(1.0f);
    carMatrix = glm::translate(carMatrix, car.position);
    carMatrix = glm::rotate(carMatrix, glm::radians(car.yaw + 360.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    carMatrix = glm::scale(carMatrix, glm::vec3(0.2f));

    shader.setMat4("model", carMatrix);

    bool hasTex = carModel.textureID != 0;
    shader.setBool("hasTexture", hasTex);

    if (hasTex)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, carModel.textureID);
        shader.setInt("tex0", 2);
    }

    glBindVertexArray(carModel.VAO);
    glDrawArrays(GL_TRIANGLES, 0, carModel.indexCount);
    glBindVertexArray(0);

    glEnable(GL_CULL_FACE);
}

void updateCameraFollowCar(const Car& car, float& camX, float& camY, float& camZ)
{
    float camDist = 1.5f;
    float camHeight = 1.0f;

    camX = car.position.x - camDist * sin(glm::radians(car.yaw));
    camY = car.position.y + camHeight;
    camZ = car.position.z - camDist * cos(glm::radians(car.yaw));
}

void updateDeliveryCarContour(Car& car, float cityOffsetX, float cityOffsetZ)
{
    static glm::vec3 corners[4];
    static bool initialized = false;

    if (!initialized) {
        float y = car.position.y;

        corners[0] = glm::vec3(cityOffsetX + 1.0f, y, cityOffsetZ + 2.0f);   // left-up
		corners[1] = glm::vec3(cityOffsetX + 12.0f, y, cityOffsetZ + 2.0f);   // right-up
		corners[2] = glm::vec3(cityOffsetX + 12.0f, y, cityOffsetZ + 12.0f);  // right-down
		corners[3] = glm::vec3(cityOffsetX + 1.0f, y, cityOffsetZ + 12.0f);  // left-down

        initialized = true;
    }

    glm::vec3 target = corners[car.targetCorner];
    glm::vec3 dir = target - car.position;
    dir.y = 0.0f;

    float dist = glm::length(dir);

    if (dist < 0.15f) {
        car.targetCorner = (car.targetCorner + 1) % 4;
        target = corners[car.targetCorner];
        dir = target - car.position;
        dir.y = 0.0f;
    }

    if (glm::length(dir) > 0.0001f) {
        dir = glm::normalize(dir);
    }

    car.speed = 0.03f;
    car.position += dir * car.speed;

    car.yaw = glm::degrees(atan2(dir.x, dir.z));
}



void updateVanCarContour(Car& car, float cityOffsetX, float cityOffsetZ)
{
    static glm::vec3 corners[4];
    static bool initialized = false;

    if (!initialized) {
        float y = car.position.y;

        corners[0] = glm::vec3(cityOffsetX + 1.0f, y, cityOffsetZ  + 7.0f);   // left-up
        corners[1] = glm::vec3(cityOffsetX + 10.0f, y, cityOffsetZ + 7.0f);   // right-up
        corners[2] = glm::vec3(cityOffsetX + 10.0f, y, cityOffsetZ + 12.0f);  // right-down
		corners[3] = glm::vec3(cityOffsetX + 1.0f, y, cityOffsetZ + 12.0f);  //  left-down

        initialized = true;
    }

    glm::vec3 target = corners[car.targetCorner];
    glm::vec3 dir = target - car.position;
    dir.y = 0.0f;

    float dist = glm::length(dir);

    if (dist < 0.15f) {
        car.targetCorner = (car.targetCorner + 1) % 4;
        target = corners[car.targetCorner];
        dir = target - car.position;
        dir.y = 0.0f;
    }

    if (glm::length(dir) > 0.0001f) {
        dir = glm::normalize(dir);
    }

    car.speed = 0.03f;
    car.position += dir * car.speed;

    car.yaw = glm::degrees(atan2(dir.x, dir.z));
}