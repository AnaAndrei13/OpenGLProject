#pragma once
#include "shader.h"
#include "model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern float cityOffsetX;
extern float cityOffsetZ;

enum TileType {
    EMPTY = 0,
    STR_H = 1,
    STR_V = 2,
    CURVE_NE = 3,
    CURVE_NW = 4,
    CURVE_SE = 5,
    CURVE_SW = 6,
    ROUNDABOUT = 12,
    HOUSE_S = 13,
    HOUSE_L = 14,
    LIGHT_TILE = 15,
    CONE_TILE = 16,
    PARK = 17,
	ST_H = 18,
	ST_V = 19,
	CONSTRUCTION_L = 20,
    SIGN=21,
	CROSS_INTERSECTION = 22,
	CROSS_ROAD = 23
};

static const int MAP_COLS = 14;
static const int MAP_ROWS = 14;

static const int cityMap[MAP_ROWS][MAP_COLS] = {
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, //   0
    { 0,  0,  0,  0,  0,  14,  0,  0,  0,  0,  0,  0,  0,  0 }, //   1 
    { 0,  3,  1,  1,  1,  22,  1,  1,  1,  1,  22, 1,  4,  0 }, //   2
    { 0,  2,  0,  0,  15,  0,  0,  0,  0, 15,  2,  0,  2,  0 }, //   3
    { 0, 22,  1,  1,  20, 12,  0,  1,  1,  1,  22, 0,  2,  0 }, //   4
    { 0,  2,  0,  15, 0,   0,  0,  0,  0,  0 , 2,  0,  2,  0 },  //  5
    { 0,  2,  0,  0,  16,  2,  0,  0,  13, 0,  2,  0,  2,  0 }, //   6
	{ 0,  22, 1,  1,  1,  23,  1,  1,  1,  1, 22,  0,  2,  0 },  //  7
	{ 0,  2,  0,  0,  0,   2,  15, 0,  0,  0,  2,  0,  2,  0 }, //   8
    { 0,  2,  0,  14, 0,   2,  0,  13, 0,  0,  2,  0,  2,  0 }, //   9
    { 0,  2,  0,  0,  0,   2,  0,  0,  14, 0,  2,  0,  2,  0 }, //  10
    { 0,  2,  0,  0,  0,   2,  0,  0,  0,  0,  2,  0,  2,  0 }, //  11
    { 0,  5,  1,  1,  1,  22,  1,  1,  1,  1,  22, 1,  6,  0 }, //  12
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, //  13
}; 

static const int objectsMap[MAP_ROWS][MAP_COLS] = {
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },   // 0
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },   // 1
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },   // 2
    { 0,  0,  0,  0,  0,   20,  0,  0,  0,  0,  0,  0,  0,  0 },  // 3 
    { 0,  0,  0,  0,  21,   0,  0,  0,  0,  0,  0,  0,  0,  0 },  // 4 
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },   // 5 
    { 0,  0,  0,  0,  0,   16,  0,  0,  0,  0,  0,  0,  0,  0 },  // 6 
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },   // 7 
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },   // 8
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },  //  9 
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },  // 10
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },  // 11
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },  // 12 
    { 0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0 },  // 13
};


struct CityModels {
    Model& roadStraight;
    Model& roadStraightBarrier;
    Model& roadCurve;
    Model& roadIntersection;
    Model& roadCurveIntersection;
    Model& roadRoundabout;
    Model& roundaboutBarrier;
    Model& constructionCone;
    Model& lightSquare;
    Model& house;
    Model& roadBend;
    Model& roadBendBarrier;
	Model& constructionLight;
	Model& crossIntersection;
	Model& crossIntersectionBarrier;
	Model& crossRoad;
    Model& sign;
};

// single definition in city.cpp
void renderCityGrid(
    Shader& shader,
    CityModels& m,
    float        bottomY,
    float        offsetX,
    float        offsetZ
   );