#include <wasm_simd128.h>
#include "world.h"
#include "stdlib.h"

World *createWorld() {
    World *world = customMalloc(sizeof(World));
    world->sphereCount = 0;
    world->light = wasm_f32x4_const(0, 0, 0, 0);
    world->camera = wasm_i32x4_const(0, 0, 0, 0);

    return world;
}

void addSphere(World *world, float x, float y, float z, float radius) {
    Sphere *sphere = customMalloc(sizeof(Sphere));

    sphere->position = wasm_f32x4_make(x, y, z, 0);
    sphere->radius = radius;

    addSphereToWorld(world, sphere);
}

void setLight(World *world, float x, float y, float z) {
    world->light = wasm_f32x4_make(x, y, z, 0);
}

void moveCamera(World *world, float x, float y, float z) {
    world->camera = wasm_f32x4_add(world->camera, wasm_f32x4_make(x, y, z, 0));
}

/*
 *   *** PRIVATE FUNCTIONS ***
 */

static void addSphereToWorld(World *world, Sphere *sphere) {
    // "realloc"
    Sphere **newSphereArray = customMalloc(sizeof(Sphere *) * (++world->sphereCount));
    for (int i = 0; i < world->sphereCount; i++) {
        newSphereArray[i] = world->spheres[i];
    }

    // add the new sphere
    newSphereArray[world->sphereCount - 1] = sphere;

    // update the world's array
    world->spheres = newSphereArray;
}
