// Minimal declarations for building Bullet (C++) without a C library; written for this project.
struct timeval { long tv_sec; long tv_usec; }; extern "C" int gettimeofday(struct timeval *, void *);
