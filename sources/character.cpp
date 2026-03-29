#include "character.h"
#include "city.h"

void initCharacter(Character& character, float x, float y, float z)
{
    character.position = glm::vec3(x, y, z);
    character.yaw = (float)(rand() % 360);
    character.speed = 0.0035f;
    character.changeDirTimer = 80.0f;
}

void updateCharacter(Character& character, float cityOffsetX, float cityOffsetZ)
{
    character.changeDirTimer -= 1.0f;

    if (character.changeDirTimer <= 0.0f) {
        character.yaw += (float)((rand() % 121) - 60);
        character.changeDirTimer = 50.0f + (rand() % 80);
    }

    glm::vec3 prevPos = character.position;

    character.position.x += sin(glm::radians(character.yaw)) * character.speed;
    character.position.z += cos(glm::radians(character.yaw)) * character.speed;

    if (!isWalkableTile(character.position.x, character.position.z, cityOffsetX, cityOffsetZ)) {
        character.position = prevPos;
        character.yaw = (float)(rand() % 360);
        character.changeDirTimer = 20.0f + (rand() % 60);
    }
}

bool isWalkableTile(float x, float z, float cityOffsetX, float cityOffsetZ)
{
    int col = (int)round(x - cityOffsetX);
    int row = (int)round(z - cityOffsetZ);

    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
        return false;

    return cityMap[row][col] == EMPTY;
}