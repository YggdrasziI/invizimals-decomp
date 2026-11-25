// Minimal declarations for building Bullet (C++) without a C library; written for this project.
#ifndef _STDLIB_H
#define _STDLIB_H
#include <stddef.h>
extern "C" { void *malloc(size_t); void *realloc(void *, size_t); void free(void *); void exit(int); int abs(int);
void qsort(void *, size_t, size_t, int (*)(const void *, const void *)); }
#endif
