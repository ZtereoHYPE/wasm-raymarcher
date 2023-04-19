#include <wasm_simd128.h>
#include "world.h"
#include "lib_malloc.h"

World *createWorld() {
    World *world = customMalloc(sizeof(World));
    world->sphereCount = 0;
    world->light = wasm_f32x4_const(0, 0, 0, 0);
    // world->camera = wasm_i32x4_const(0, 0, 0, 0);

    return world;
}

void createSphere(World *world, float x, float y, float z, float radius) {
    Sphere *sphere = customMalloc(sizeof(Sphere));

    sphere->position = wasm_f32x4_make(x, y, z, 0);
    sphere->radius = radius;

    _addSphereToWorld(world, sphere);
}

/*
 *   *** PRIVATE FUNCTIONS ***
 */

void _addSphereToWorld(World *world, Sphere *sphere) {
    // "realloc"
    Sphere **newSphereArray = customMalloc(sizeof(Sphere *) * (world->sphereCount + 1));
    for (int i = 0; i < world->sphereCount; i++) {
        newSphereArray[i] = world->spheres[i];
    }

    // add the new sphere
    newSphereArray[world->sphereCount] = sphere;

    // update the world's array
    world->spheres = newSphereArray;
}

void setLight(World *world, float x, float y, float z) {
    world->light = wasm_f32x4_make(x, y, z, 0);
}