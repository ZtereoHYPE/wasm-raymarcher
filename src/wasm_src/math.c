#include "math.h"
#include "stdlib.h"

/*
 * STANDARD MATH
 */
double sqrt(double n) { return __builtin_sqrt(n); }

double absVal(double n) { return __builtin_fabs(n); }

double sin(double n) { return __builtin_sin(n); }

double cos(double n) { return __builtin_cos(n); }

/*
 * WASM MATH
 */
double wasm_dot(v128_t a, v128_t b) {
    v128_t c = wasm_f32x4_mul(a, b);
    
    float dot = 0;
    dot += wasm_f32x4_extract_lane(c, 0);
    dot += wasm_f32x4_extract_lane(c, 1);
    dot += wasm_f32x4_extract_lane(c, 2);
   
    return dot;
}

/*
 * MATRIX MATH
 */
Vector3f mult_matrix_vec(Matrix3f m, Vector3f v) {
    // float *mv = m.values;
    // float *vv = v.values;

    Vector3f result = {0, 0, 0};
    
    // TODO: this could be manually unrolled. Does it improve sped?
    // this loop is the problem causing things not to "recurse too much"
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.values[j] += m.values[i * 3 + j] * v.values[j];
        }
    }
    // result.values[0] += m.values[0 * 3 + 0] * v.values[0];
    // result.values[1] += m.values[0 * 3 + 1] * v.values[1];
    // result.values[2] += m.values[0 * 3 + 2] * v.values[2];

    // result.values[0] += m.values[1 * 3 + 0] * v.values[0];
    // result.values[1] += m.values[1 * 3 + 1] * v.values[1];
    // result.values[2] += m.values[1 * 3 + 2] * v.values[2];

    // result.values[0] += m.values[2 * 3 + 0] * v.values[0];
    // result.values[1] += m.values[2 * 3 + 1] * v.values[1];
    // result.values[2] += m.values[2 * 3 + 2] * v.values[2];


    logNumber(result.values[0]);

    return result;
}

