// Minimal declarations for building Bullet (C++) without a C library; written for this project.
#ifndef _MATH_H
#define _MATH_H
extern "C" {
float sqrtf(float); float fabsf(float); float cosf(float); float sinf(float); float tanf(float);
float acosf(float); float asinf(float); float atanf(float); float atan2f(float, float);
float expf(float); float logf(float); float powf(float, float); float fmodf(float, float);
double sqrt(double); double fabs(double); double cos(double); double sin(double); double tan(double);
double acos(double); double asin(double); double atan(double); double atan2(double, double);
double exp(double); double log(double); double pow(double, double); double fmod(double, double);
}
#endif
