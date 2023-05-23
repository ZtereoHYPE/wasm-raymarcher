#include "stdlib.h"
#include <wasm_simd128.h>

#define WASM_PAGE_SIZE 65536

extern unsigned char __heap_base;
void *bump_pointer = &__heap_base;

int currentPages = 1;

// we do not have access to malloc, so allocate memory on the llvm heap   
void *customMalloc(int size) {
    void *r = bump_pointer;
    bump_pointer = (void *)bump_pointer + size;

    // if we are out of memory, grow the heap. the 2048 stays until we figure out what uses the extra heap space (1024??)
    if (getHeapUsage() + 2048 >= currentPages * WASM_PAGE_SIZE) {
        int newPages = (size / WASM_PAGE_SIZE) + 1;
        growMemory(newPages);
    }

    return r;
}

// since we have no free, we will have to wipe the memory manually after each call/frame maybe...?
void wipeMemory() {
    bump_pointer = (void *) &__heap_base;
}

void popMemory(int size) {
    bump_pointer = (void *)bump_pointer - size;
}

// returns the distance between the heap base and the bump pointer, in bytes 
int getHeapUsage() {
    return (int) bump_pointer - (int) &__heap_base;
}

// memory intrinsics wrappers
void *memset(void *ptr, int value, unsigned long num) {
    return __builtin_memset(ptr, value, num);
}

void *memcpy(void *destination, const void *source, unsigned long n) {
    return __builtin_memcpy(destination, source, n);
}

void *memmove(void *destination, const void *source, unsigned long num) {
    return __builtin_memmove(destination, source, num);
}

int memcmp(const void *ptr1, const void *ptr2, unsigned long num) {
    return __builtin_memcmp(ptr1, ptr2, num);
}

void *memchr(const void *ptr, int value, unsigned long num) {
    return __builtin_memchr(ptr, value, num);
}

void growMemory(int pages) {
    __builtin_wasm_memory_grow(0, pages);
    currentPages += pages;
}

// math functions
double sqrt(double n) {
    return __builtin_sqrt(n);
}

double dot(v128_t a, v128_t b) {
    v128_t c = wasm_f32x4_mul(a, b);
    
    float dot = 0;
    dot += wasm_f32x4_extract_lane(c, 0);
    dot += wasm_f32x4_extract_lane(c, 1);
    dot += wasm_f32x4_extract_lane(c, 2);
   
    return dot;
}

double absVal(double n) {
    return __builtin_fabs(n);
}