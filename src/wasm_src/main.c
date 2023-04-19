#include <wasm_simd128.h>
#include "lib_malloc.h"
#include "world.h"


int *rayMarch(int resolution, World *world) {
    // 3 channels per pixel
    int *pixels = customMalloc(resolution * resolution * 3);

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            pixels[i * resolution + j] = i*5;
            pixels[i * resolution + j + 1] = j*5;
            pixels[i * resolution + j + 2] = 0;
        }
    }

    return pixels;
}

// int sumInts(int x, int y) {
//     v128_t vx = wasm_i32x4_splat(x);
//     v128_t vy = wasm_i32x4_splat(y);

//     v128_t vprod = wasm_i32x4_mul(vx, vy);

//     v128_t vsum = wasm_i32x4_add(vx, vy);

//     return wasm_i32x4_extract_lane(vprod, 1);
// }

// int *returnArrayOfLength(int n) {
//     int *arr = customMalloc(n * sizeof(int));
//     for (int i = 0; i < n; i++) {
//         arr[i] = i;
//     }
//     return arr;
// }
