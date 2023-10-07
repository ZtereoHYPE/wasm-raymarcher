#include "stdlib.h"
#include "world.h"
#include "math.h"

float *rasterize(const unsigned int resolution, World *world) {
    // 3 channels per pixel + clear it
    float *pixels = customMalloc(resolution * resolution * 3 * sizeof(float));
    memset(pixels, 0, resolution * resolution * 3 * sizeof(float));

    // is this how you stack-allocate?
    float projectedVertices[world->vertexCount * 2];

    float rv = world->camera.rv;
    float rh = world->camera.rh;
    Matrix3f cameraMatrix = {
        cos(rh),            0,          -sin(rh),
        -sin(rv)*sin(rh),   cos(rv),    -sin(rv)*cos(rh),
        cos(rv)*sin(rh),    sin(rv),    cos(rv)*cos(rh)
    };

    float fov = 0.5f;

    for (int i = 0; i < world->vertexCount; i++) {
        // logNumber(sin(-4));
        Vector3f vertex = world->vertices[i * 2];
        Vector3f projected = mult_matrix_vec(cameraMatrix, vertex);

        float px = projected.values[0];
        float py = projected.values[1];
        float pz = projected.values[2];

        projectedVertices[i * 2 + 0] = fov * px / pz;
        projectedVertices[i * 2 + 1] = fov * py / pz;
    }

    // // for (int i = 0; i < world->vertexCount / 2; i++) {
    // //     float x1 = projectedVertices[i*4 + 0],
    // //           y1 = projectedVertices[i*4 + 1],
    // //           x2 = projectedVertices[i*4 + 2],
    // //           y2 = projectedVertices[i*4 + 3];

    // //     // check if both are off-screen to avoid doin anything then
    // //     if ()
    // // }

    for (int i = 0; i < world->vertexCount; i++) {
        float x = projectedVertices[i*2 + 0],
              y = projectedVertices[i*2 + 1];

        // // avoid off-screen points
        // if (x > resolution || x < 0 || y > resolution || y < 0)
        //     continue;
        
        // color the pixel
        // pixels[3 * (y * resolution + x) + 0] = 1.0;
        // pixels[3 * (y * resolution + x) + 1] = 1.0;
        // pixels[3 * (y * resolution + x) + 2] = 1.0;
        // logNumber(y);
    }

    // doesn't actually clear it, it's just so that at the next render we're not leaking hundreds of MBs
    popMemory(resolution * resolution * 3 * sizeof(float));
    return pixels;
}
