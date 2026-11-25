// Minimal declarations for building Bullet (C++) without a C library; written for this project.
#ifndef _STDIO_H
#define _STDIO_H
#include <stddef.h>
extern "C" { int printf(const char *, ...); int sprintf(char *, const char *, ...); }
#endif
