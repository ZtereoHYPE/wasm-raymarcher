#ifndef LIBMALLOC_H
#define LIBMALLOC_H

void *customMalloc(int size);
void wipeMemory();
int getHeapUsage();

#endif