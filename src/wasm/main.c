#include <wasm_simd128.h>

int sumInts(int x, int y) {
    v128_t vx = wasm_i32x4_splat(x);
    v128_t vy = wasm_i32x4_splat(y);

    v128_t vsum = wasm_i32x4_add(vx, vy);

    return wasm_i32x4_extract_lane(vsum, 0);
}
