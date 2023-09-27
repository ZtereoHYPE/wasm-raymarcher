#pragma once

#include <wasm_simd128.h>
#include "math.h"

typedef struct {
    float x;
    float y;
    float z;
    float rh; // horizontal angle in radians
    float rv; // vertical angle in radians
} Camera;

typedef struct {
    Vector3f *vertices;
    int vertexCount;
    Camera camera;
} World;

World *createWorld();
void addLine(World *world, float x1, float y1, float z1, float x2, float y2, float z2);

/* private stuff */
static void addVertexToWorld(World *world, Vector3f v);
