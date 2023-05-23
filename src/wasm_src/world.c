#include <wasm_simd128.h>
#include "world.h"
#include "stdlib.h"

World *createWorld() {
    World *world = customMalloc(sizeof(World));
    world->sphereCount = 0;
    world->planeCount = 0;
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

void addPlane(World *world, float x, float y, float z, float nx, float ny, float nz) {
    Plane *plane = customMalloc(sizeof(Plane));

    plane->position = wasm_f32x4_make(x, y, z, 0);
    plane->normal = wasm_f32x4_make(nx, ny, nz, 0);

    addPlaneToWorld(world, plane);
}

void setLight(World *world, float x, float y, float z) {
    world->light = wasm_f32x4_make(x, y, z, 0);
    // normalise the vector
    float lightLength = sqrt(
        x * x +
        y * y +
        z * z
    );

    world->light = wasm_f32x4_div(world->light, wasm_f32x4_splat(lightLength));
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

static void addPlaneToWorld(World *world, Plane *plane) {
    // "realloc"
    Plane **newPlaneArray = customMalloc(sizeof(Plane *) * (++world->planeCount));
    for (int i = 0; i < world->planeCount; i++) {
        newPlaneArray[i] = world->planes[i];
    }

    // add the new plane
    newPlaneArray[world->planeCount - 1] = plane;

    // update the world's array
    world->planes = newPlaneArray;
}