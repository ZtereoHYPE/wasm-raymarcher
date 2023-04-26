#include <wasm_simd128.h>
#include "stdlib.h"
#include "world.h"

// todo: optimise w/ SIMD distance calculation
double getSurfaceDistance(v128_t location, World *world) {
    double smallestDistance = DOUBLE_MAX, currentDistance;
    // world->spheres[1000000] = 0;

    for (int i = 0; i < world->sphereCount; i++) {
        float locX = wasm_f32x4_extract_lane(location, 0);
        float locY = wasm_f32x4_extract_lane(location, 1);
        float locZ = wasm_f32x4_extract_lane(location, 2);
        float sphereX = wasm_f32x4_extract_lane(world->spheres[i]->position, 0);
        float sphereY = wasm_f32x4_extract_lane(world->spheres[i]->position, 1);
        float sphereZ = wasm_f32x4_extract_lane(world->spheres[i]->position, 2);

        currentDistance = sqrt(
            (locX - sphereX) * (locX - sphereX) +
            (locY - sphereY) * (locY - sphereY) +
            (locZ - sphereZ) * (locZ - sphereZ)
        ) - world->spheres[i]->radius;

        if (currentDistance < smallestDistance) {
            smallestDistance = currentDistance;
        }
    }

    return smallestDistance;
}

int *rayMarch(const unsigned int resolution, World *world) {
    // 3 channels per pixels
    int *pixels = customMalloc(resolution * resolution * 3 * sizeof(int));

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            // clear the pixel
            pixels[(i * resolution + j) * 3] = 0;
            pixels[(i * resolution + j) * 3 + 1] = 0;
            pixels[(i * resolution + j) * 3 + 2] = 100;

            // calculate the UV coordinates and init vectors
            float u = ((float) i / ((float)resolution / 2)) - 1;
            float v = ((float) j / ((float)resolution / 2)) - 1;

            v128_t uvVector = wasm_f32x4_make(u, v, 1, 0);
            // normalise the vector
            float uvVectorLength = sqrt(
                u * u +
                v * v +
                1 * 1
            );
            uvVector = wasm_f32x4_div(uvVector, wasm_f32x4_splat(uvVectorLength));

            v128_t rayLocation = world->camera;
            double closestDistance = DOUBLE_MAX;
            int loops = 0;

            // march the ray forwards
            while (closestDistance > 1 && loops < 1000) {
                closestDistance = getSurfaceDistance(rayLocation, world);
                rayLocation = wasm_f32x4_add(rayLocation, uvVector);
                loops++;
            }

            // if we hit a surface, calculate the color
            if (closestDistance <= 1) {
                pixels[(i * resolution + j) * 3] = 255;
                pixels[(i * resolution + j) * 3 + 1] = 255;
                pixels[(i * resolution + j) * 3 + 2] = 255;
            }
        }
    }

    // doesn't actually clear it, it's just so that at the next render we're not leaking hundreds of MBs
    popMemory(resolution * resolution * 3 * sizeof(int));
    return pixels;
}
