#ifndef LIBC_STDDEF_H
#define LIBC_STDDEF_H

/* Minimal declarations for the library sources built with the game (src/expat);
 * not the headers of the original toolchain. */

typedef unsigned int size_t;
typedef int ptrdiff_t;

#ifndef NULL
#define NULL ((void *)0)
#endif

#define offsetof(type, member) __builtin_offsetof(type, member)

#endif
