#ifndef WORLD_H
#define WORLD_H

#include <wasm_simd128.h>

typedef struct {
    v128_t position;
    float radius;
} Sphere;

typedef struct {
    Sphere **spheres;
    int sphereCount;
    v128_t light;
    v128_t camera;
} World;

World *createWorld();
void addSphere(World *world, float x, float y, float z, float radius);
void setLight(World *world, float x, float y, float z);

/* private stuff */
static void addSphereToWorld(World *world, Sphere *sphere);

#endif