#include "stdlib.h"
#include "world.h"
#include "math.h"

void paint(float *pixels, int res, int x, int y) {
    if (x > res || x < 0 || y > res || y < 0) {
        return;
    }

    pixels[3 * (x + res * y) + 0] = 1.0;
    pixels[3 * (x + res * y) + 1] = 1.0;
    pixels[3 * (x + res * y) + 2] = 1.0;
}

void traceLine(float *pixels, int resolution, int x1, int y1, int x2, int y2) {
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;

    int j, j_end, i, i_end, step, deltaI, deltaJ;
    int *x, *y;

    float slope = deltaY / (float) deltaX;

    // we iterate over X axys
    if (absVal(slope) < 1) {
        i = x1;
        i_end = x2;
        j = y1;
        j_end = y2;
        deltaI = deltaX;
        deltaJ = deltaY;
        x = &i;
        y = &j;

    // we iterate over Y axys
    } else {
        i = y1;
        i_end = y2;
        j = x1;
        j_end = x2;
        deltaI = deltaY;
        deltaJ = deltaX;
        x = &j;
        y = &i;
    }

    // we always draw LTR
    if (i > i_end) {
        int swap = i;
        i = i_end;
        i_end = swap;

        swap = j;
        j = j_end;
        j_end = swap;

        deltaI = -deltaI;
        deltaJ = -deltaJ;
    }
    
    step = slope > 0 ? 1 : -1;

    int j0 = j;
    int i0 = i;
    while (i < i_end) {
        int actualI = i - i0;
        int pos = 2 * deltaJ * actualI + 2 * deltaI * j0 - deltaI - 2 * deltaI * j;

        if ((step > 0 && pos > 0) || (step < 0 && pos < 0)) {
            j += step;
        }

        paint(pixels, resolution, *x, *y);

        i++;
    }
}

float clipToCoords(float clip, int res) {
    return ((clip + 1.0f) / 2.0f) * res;
}

float *rasterize(const unsigned int resolution, World *world) {
    // 3 channels per pixel + clear it
    float *pixels = customMalloc(resolution * resolution * 3 * sizeof(float));
    memset(pixels, 0, resolution * resolution * 3 * sizeof(float));

    // this is evil allocation
    float projectedVertices[world->vertexCount * 2];

    float rv = world->camera.rv;
    float rh = world->camera.rh;
    Matrix3f cameraMatrix = {
        cos(rh),            0,          -sin(rh),
        -sin(rv)*sin(rh),   cos(rv),    -sin(rv)*cos(rh),
        cos(rv)*sin(rh),    sin(rv),    cos(rv)*cos(rh)
    };

    float fov = 0.5f;

    int proj_amt = 0;

    for (int i = 0; i < world->vertexCount; i++) {
        Vector3f vertex = world->vertices[i];
        Vector3f projected = mult_matrix_vec(cameraMatrix, vertex);

        float px = projected.values[0] - world->camera.x;
        float py = projected.values[1] - world->camera.y;
        float pz = projected.values[2] - world->camera.z;

        logNumber(pz);

        if (pz < 1) {
            continue;
        }

        projectedVertices[i*2 + 0] = fov * px / pz;
        projectedVertices[i*2 + 1] = fov * py / pz;

        proj_amt++;
    }

    for (int i = 0; i < proj_amt / 2; i++) {
        float x1 = projectedVertices[i*4 + 0],
            y1 = projectedVertices[i*4 + 1],
            x2 = projectedVertices[i*4 + 2],
            y2 = projectedVertices[i*4 + 3];

        int screenX1 = clipToCoords(x1, resolution);
        int screenY1 = clipToCoords(y1, resolution);

        int screenX2 = clipToCoords(x2, resolution);
        int screenY2 = clipToCoords(y2, resolution);

        // TODO: check if both are off-screen to avoid doin anything then
        //  maybe clamp to edge of screen the second?

        traceLine(pixels, resolution, screenX1, screenY1, screenX2, screenY2);
    }

    // doesn't actually clear it, it's just so that at the next render we're not leaking hundreds of MBs
    popMemory(resolution * resolution * 3 * sizeof(float));
    return pixels;
}