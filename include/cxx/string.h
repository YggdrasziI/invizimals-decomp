// Minimal declarations for building Bullet (C++) without a C library; written for this project.
#ifndef _STRING_H
#define _STRING_H
#include <stddef.h>
extern "C" {
void *memcpy(void *, const void *, size_t); void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t); int memcmp(const void *, const void *, size_t);
size_t strlen(const char *); int strcmp(const char *, const char *); char *strcpy(char *, const char *);
}
#endif
