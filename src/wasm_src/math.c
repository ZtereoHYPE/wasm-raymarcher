#include "math.h"
#include "stdlib.h"

__attribute__((import_module("env"), import_name("jssin"))) double jsSin(double n);

/*
 * STANDARD MATH
 */
double truncate(double n) {
    return (double) (int) n;
}

double fmod(double n, double m) {
    return n - truncate(n / m) * m;
}

// double lookup_table[LOOKUP_SIZE];
// int initialized_lookup = 0;

// void init_lookup_table() {
//     for (int i = 0; i < LOOKUP_SIZE; i++) {
//         double angle = (PI / 2) * ((double)i / LOOKUP_SIZE);
//         logNumber(angle);
//         lookup_table[i] = jsSin(angle);
//     }
// }

double sqrt(double n) { return __builtin_sqrt(n); }

double absVal(double n) { return __builtin_fabs(n); }

// double sin(double n) {
    // logNumber(12);
    // if (unlikely(!initialized_lookup))
    //     init_lookup_table();

    // n = fmod(n, 2*PI);

    // double factor = 1;

    // double n_shift;
    // if (n > 3.0/2.0 * PI) {
    //     n_shift = 2 * PI - n;
    // } else if (n > PI) {
    //     factor = -1;
    //     n_shift = n - PI;
    // } else if (n > PI/2.0) {
    //     factor = -1;
    //     n_shift = PI - n;
    // } else {
    //     n_shift = n;
    // }

    // int idx = n_shift / (2 / PI) * LOOKUP_SIZE;
    // return factor * lookup_table[idx]; 
// }

// double cos(double n) { 
//     if (unlikely(!initialized_lookup))
//         init_lookup_table();

//     return 1;
// }

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
    // for (int i = 0; i < 3; i++) {
    //     for (int j = 0; j < 3; j++) {
    //         result.values[j] += m.values[i * 3 + j] * v.values[j];
    //     }
    // }

    // logNumber(m.values[0 * 3 + 0]);

    result.values[0] += m.values[0 * 3 + 0] * v.values[0];
    result.values[1] += m.values[0 * 3 + 1] * v.values[1];
    result.values[2] += m.values[0 * 3 + 2] * v.values[2];

    result.values[0] += m.values[1 * 3 + 0] * v.values[0];
    result.values[1] += m.values[1 * 3 + 1] * v.values[1];
    result.values[2] += m.values[1 * 3 + 2] * v.values[2];

    result.values[0] += m.values[2 * 3 + 0] * v.values[0];
    result.values[1] += m.values[2 * 3 + 1] * v.values[1];
    result.values[2] += m.values[2 * 3 + 2] * v.values[2];



    return result;
}

