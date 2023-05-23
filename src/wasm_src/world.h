#ifndef WORLD_H
#define WORLD_H

#include <wasm_simd128.h>

typedef enum {
    SphereShape,
    PlaneShape
} ShapeType;

typedef struct {
    v128_t position;
    float radius;
} Sphere;

typedef struct {
    v128_t position;
    v128_t normal;
} Plane;

typedef struct {
    ShapeType type;
    void *shape;
} Shape;

typedef struct {
    Sphere **spheres;
    Plane **planes;
    int sphereCount;
    int planeCount;
    v128_t light;
    v128_t camera;
} World;

World *createWorld();
void addSphere(World *world, float x, float y, float z, float radius);
void setLight(World *world, float x, float y, float z);

/* private stuff */
static void addSphereToWorld(World *world, Sphere *sphere);
static void addPlaneToWorld(World *world, Plane *plane);

#endif