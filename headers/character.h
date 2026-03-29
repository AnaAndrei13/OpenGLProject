#pragma once
#include <glm/glm.hpp>

struct Character {
    glm::vec3 position;
    float yaw;
    float speed;
    float changeDirTimer;
};

void updateCharacter(Character& character, float cityOffsetX, float cityOffsetZ);
void initCharacter(Character& character, float x, float y, float z);
bool isWalkableTile(float x, float z, float cityOffsetX, float cityOffsetZ);