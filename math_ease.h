#ifndef MATH_EASE_H
#define MATH_EASE_H

typedef void (*ease_func)(float t, float b, float c, float d);

void ease_linear(float t, float b, float c, float d);

#endif
