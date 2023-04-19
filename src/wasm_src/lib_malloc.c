#include "lib_malloc.h"

extern unsigned char __heap_base;
void *bump_pointer = &__heap_base;

// we do not have access to malloc, so allocate memory on the llvm heap
void *customMalloc(int size) {
    void *r = bump_pointer;
    bump_pointer = (void *)bump_pointer + size;
    return r;
}

// since we have no free, we will have to wipe the memory manually after each call/frame maybe...?
void wipeMemory() {
    bump_pointer = (void *) &__heap_base;
}

// returns the distance between the heap base and the bump pointer, in bytes 
int getHeapUsage() {
    return (int) bump_pointer - (int) &__heap_base;
}