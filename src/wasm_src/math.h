#pragma once

#include <wasm_simd128.h>

#define PI 3.1415926536
#define LOOKUP_SIZE 100

// standard math functions
double truncate(double n);
double fmod(double n, double m);
double sqrt(double n);
double absVal(double n);
double sin(double n);
double cos(double n);

// wasm math functions
double wasm_dot(v128_t a, v128_t b);

// matrix math
typedef struct {
    float values[9];
} Matrix3f;

typedef struct {
    float values[3];
} Vector3f;

Vector3f mult_matrix_vec(Matrix3f m, Vector3f v);