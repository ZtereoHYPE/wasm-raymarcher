#ifndef STDLIB_H
#define STDLIB_H

#include <wasm_simd128.h>

#define WASM_PAGE_SIZE 65536
#define DOUBLE_MAX 1.7976931348623157e+308
#define INT_MAX 2147483647
#define NULL 0

// logging
__attribute__((import_module("env"), import_name("jslog"))) void logNumber(double n);

// memory
void *customMalloc(int size);
void wipeMemory();
void popMemory(int size);
int getHeapUsage();
void *memset(void *ptr, int value, unsigned long num);
void *memcpy(void *destination, const void *source, unsigned long n);
void *memmove(void *destination, const void *source, unsigned long num);
int memcmp(const void *ptr1, const void *ptr2, unsigned long num);
void growMemory(int pages);

// math
double sqrt(double n);
double dot(v128_t a, v128_t b);

#endif