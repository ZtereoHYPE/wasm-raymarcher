#include <wasm_simd128.h>
#include "stdlib.h"
#include "world.h"

double simdDistance(const v128_t v1, const v128_t v2) {
    v128_t result = wasm_f32x4_sub(v1, v2);
    result = wasm_f32x4_mul(result, result);

    double dist = wasm_f32x4_extract_lane(result, 0) +
                  wasm_f32x4_extract_lane(result, 1) +
                  wasm_f32x4_extract_lane(result, 2);

    return sqrt(dist);
}

double sphereSdf(const v128_t location, const Sphere *sphere) {
    return simdDistance(location, sphere->position) - sphere->radius;
}

double planeSdf(const v128_t location, const Plane *plane) {
    return absVal(dot(plane->normal, wasm_f32x4_sub(location, plane->position)));
}

double getSurfaceDistance(v128_t location, World *world, Shape **closest) {
    double smallestDistance = DOUBLE_MAX, currentDistance;

    if (*closest == NULL)
        *closest = customMalloc(sizeof(Shape));

    for (int i = 0; i < world->sphereCount; i++) {
        Sphere *sphere = world->spheres[i];
        currentDistance = sphereSdf(location, sphere);

        if (currentDistance < smallestDistance) {
            smallestDistance = currentDistance;
            (*closest)->shape = world->spheres[i];
            (*closest)->type = SphereShape;
        }
    }

    for (int i = 0; i < world->planeCount; i++) {
        Plane *plane = world->planes[i];
        currentDistance = planeSdf(location, plane);

        if (currentDistance < smallestDistance) {
            smallestDistance = currentDistance;
            (*closest)->shape = world->planes[i];
            (*closest)->type = PlaneShape;
        }
    }

    return smallestDistance;
}

v128_t getSurfaceNormal(const v128_t location, Shape *closest) {
    const float epsilon = 0.0001;
    float xDelta, yDelta, zDelta, dist;
    v128_t xIncr, yIncr, zIncr, normal;

    xIncr = wasm_f32x4_make(epsilon, 0, 0, 0);
    xIncr = wasm_f32x4_add(xIncr, location);

    yIncr = wasm_f32x4_make(0, epsilon, 0, 0);
    yIncr = wasm_f32x4_add(yIncr, location);

    zIncr = wasm_f32x4_make(0, 0, epsilon, 0);
    zIncr = wasm_f32x4_add(zIncr, location);

    switch(closest->type) {
        case SphereShape:
            dist = sphereSdf(location, closest->shape);
            xDelta = sphereSdf(xIncr, closest->shape) - dist;
            yDelta = sphereSdf(yIncr, closest->shape) - dist;
            zDelta = sphereSdf(zIncr, closest->shape) - dist;
            break;
        case PlaneShape:
            dist = planeSdf(location, closest->shape);
            xDelta = planeSdf(xIncr, closest->shape) - dist;
            yDelta = planeSdf(yIncr, closest->shape) - dist;
            zDelta = planeSdf(zIncr, closest->shape) - dist;
            break;
    }

    normal = wasm_f32x4_make(xDelta, yDelta, zDelta, 0);

    float normalLength = sqrt(
        xDelta * xDelta +
        yDelta * yDelta +
        zDelta * zDelta
    );

    // normalise the vector
    return wasm_f32x4_div(normal, wasm_f32x4_splat(normalLength));
}

void normalShader(float *pixels, int res, int i, int j, v128_t normal) {
    normal = wasm_f32x4_mul(normal, wasm_f32x4_splat(0.5));
    normal = wasm_f32x4_add(normal, wasm_f32x4_splat(0.5));

    pixels[(i * res + j) * 3] = wasm_f32x4_extract_lane(normal, 0);
    pixels[(i * res + j) * 3 + 1] = wasm_f32x4_extract_lane(normal, 1);
    pixels[(i * res + j) * 3 + 2] = wasm_f32x4_extract_lane(normal, 2);
}

void lightShader(float *pixels, int res, int i, int j, v128_t normal, World *world) {
    v128_t color;

    float lightIntensity = dot(normal, world->light);
    lightIntensity += 0.3;
    float minLight = 0.2;

    lightIntensity = lightIntensity < minLight ? minLight : lightIntensity;

    color = wasm_f32x4_mul(color, wasm_f32x4_splat(lightIntensity));

    pixels[(i * res + j) * 3] = wasm_f32x4_extract_lane(color, 0);
    pixels[(i * res + j) * 3 + 1] = wasm_f32x4_extract_lane(color, 1);
    pixels[(i * res + j) * 3 + 2] = wasm_f32x4_extract_lane(color, 2);
}


float *rayMarch(const unsigned int resolution, World *world) {
    // 3 channels per pixels
    float *pixels = customMalloc(resolution * resolution * 3 * sizeof(float));

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            // clear the pixel
            pixels[(i * resolution + j) * 3] = 0;
            pixels[(i * resolution + j) * 3 + 1] = 0;
            pixels[(i * resolution + j) * 3 + 2] = 0;

            // calculate the UV coordinates and init vectors
            float u = ((float) i / ((float)resolution / 2)) - 1;
            float v = ((float) j / ((float)resolution / 2)) - 1;

            // calculate the ray vector
            v128_t uvVector = wasm_f32x4_make(u, v, 1, 0);
            float uvVectorLength = sqrt(
                u * u +
                v * v +
                1 * 1
            );
            uvVector = wasm_f32x4_div(uvVector, wasm_f32x4_splat(uvVectorLength));

            v128_t rayLocation = world->camera;
            double closestDistance;
            Shape *closestShape = NULL;

            // march the ray forwards
            int loops = 0;
            do {
                closestDistance = getSurfaceDistance(rayLocation, world, &closestShape);
                rayLocation = wasm_f32x4_add(rayLocation, wasm_f32x4_mul(uvVector, wasm_f32x4_splat(closestDistance)));
                loops++;
            } while (closestDistance > 0.01 && closestDistance < 1000 && loops < 256);

            // if we hit a surface, calculate the color
            if (closestDistance <= 0.01) {
                v128_t normal = getSurfaceNormal(rayLocation, closestShape);

                normalShader(pixels, resolution, i, j, normal);
                // lightShader(pixels, resolution, i, j, normal, world);
            }
        }
    }

    // doesn't actually clear it, it's just so that at the next render we're not leaking hundreds of MBs
    popMemory(resolution * resolution * 3 * sizeof(float) + sizeof(Shape));
    return pixels;
}
