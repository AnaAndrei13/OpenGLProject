#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "model.h"
#include "shader.h"

struct Car {
    glm::vec3 position;
    float yaw;
    float speed;
    float width = 1.8f;
    float length = 1.8f;
};

void initCar(Car& car, float cityOffsetX, float cityOffsetZ, float bottomY);
void renderCar(const Car& car, Model& carModel, Shader& shader);
void updateCar(Car& car, GLFWwindow* window, float cityOffsetX, float cityOffsetZ);
void updateCameraFollowCar(const Car& car, float& camX, float& camY, float& camZ);