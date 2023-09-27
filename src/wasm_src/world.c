#include <wasm_simd128.h>
#include "world.h"
#include "stdlib.h"

World *createWorld() {
    World *world = customMalloc(sizeof(World));
    world->vertexCount = 0;
    world->camera = (Camera) {
        0, 0, 0, 0, 0           // is this how struct instantiation works in C?
    };

    return world;
}

void addLine(World *world, float x1, float y1, float z1, float x2, float y2, float z2) {
    Vector3f v = (Vector3f) {x1, y1, z1};
    addVertexToWorld(world, v);

    v = (Vector3f) {x2, y2, z2};
    addVertexToWorld(world, v);
}

void moveCamera(World *world, float x, float y, float z) {
    world->camera.x += x;
    world->camera.y += y;
    world->camera.z += z;
}

void rotateCamera(World *world, float rh, float rv) {
    world->camera.rh += rh;
    world->camera.rv += rv;
}

/*
 *   *** PRIVATE FUNCTIONS ***
 */
static void addVertexToWorld(World *world, Vector3f v) {
    // "realloc"
    int newVertexCount = ++(world->vertexCount);
    Vector3f *newVertexArray = customMalloc(sizeof(Vector3f) * (newVertexCount));
    memcpy(newVertexArray, world->vertices, sizeof(Vector3f) * (newVertexCount));

    // add the new sphere
    newVertexArray[newVertexCount - 1] = v;

    // update the world's array
    world->vertices = newVertexArray;
}