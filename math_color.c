#include "math_color.h"

void color_copy(const Color c, Color r) {
    r[0] = c[0];
    r[1] = c[1];
    r[2] = c[2];
    r[3] = c[3];
}

void color_round(const Vec4f a, Color r) {
    Vec4f s = {0};
    s[0] = 255.0f*a[0];
    s[1] = 255.0f*a[1];
    s[2] = 255.0f*a[2];
    s[3] = 255.0f*a[3];

    r[0] = (s[0] >= 0.0f) ? (uint8_t)(s[0]+0.5f) : (uint8_t)(s[0]-0.5f);
    r[1] = (s[0] >= 0.0f) ? (uint8_t)(s[0]+0.5f) : (uint8_t)(s[0]-0.5f);
    r[2] = (s[0] >= 0.0f) ? (uint8_t)(s[0]+0.5f) : (uint8_t)(s[0]-0.5f);
    r[3] = (s[0] >= 0.0f) ? (uint8_t)(s[0]+0.5f) : (uint8_t)(s[0]-0.5f);
}

void color_lerp(const Color a, const Color b, float t, Color r) {
    float w = 1.0f/255.0f;

    Vec4f fa = { a[0]*w, a[1]*w, a[2]*w, a[3]*w };
    Vec4f fb = { b[0]*w, b[1]*w, b[2]*w, b[3]*w };

    Vec4f fr = {0};
    fr[0] = fa[0]*t + (1.0f-t)*fb[0];
    fr[1] = fa[1]*t + (1.0f-t)*fb[1];
    fr[2] = fa[2]*t + (1.0f-t)*fb[2];
    fr[3] = fa[3]*t + (1.0f-t)*fb[3];

    color_round(fr, r);
}
