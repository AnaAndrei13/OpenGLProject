#include "car.h"
#include "city.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

void initCar(Car& car, float cityOffsetX, float cityOffsetZ, float bottomY)
{
    car.position = glm::vec3(cityOffsetX +1.0f, bottomY + 0.01f, cityOffsetZ + 2.2f);
    car.yaw = 0.0f;
    car.speed = 0.0f;
}

void updateCar(Car& car, GLFWwindow* window, float cityOffsetX, float cityOffsetZ)
{
    glm::vec3 prevPos = car.position;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        car.speed += 0.001f;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        car.speed -= 0.001f;

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
    glm::mat4 carMatrix = glm::mat4(1.0f);
    carMatrix = glm::translate(carMatrix, car.position);
    carMatrix = glm::rotate(carMatrix, glm::radians(car.yaw + 360.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    carMatrix = glm::scale(carMatrix, glm::vec3(0.2f));

    shader.setMat4("model", carMatrix);

    glBindVertexArray(carModel.VAO);
    glDrawArrays(GL_TRIANGLES, 0, carModel.vertexCount);
    glBindVertexArray(0);
}

void updateCameraFollowCar(const Car& car, float& camX, float& camY, float& camZ)
{
    float camDist = 1.5f;
    float camHeight = 1.0f;

    camX = car.position.x - camDist * sin(glm::radians(car.yaw));
    camY = car.position.y + camHeight;
    camZ = car.position.z - camDist * cos(glm::radians(car.yaw));
}