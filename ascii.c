#include "ascii.h"

struct Character char_A() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_a() {
    static short pixels[6*6] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0, 
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_B() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_b() {
    static short pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 0, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_C() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 0,
        0, 1, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_c() {
    static short pixels[5*6] = {
        0, 0, 0, 0, 0,
        0, 0, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_D() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_d() {
    static short pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 0,
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_E() {
    static short pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 0,
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_e() {
    static short pixels[5*6] = {
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 0, 0, 0,
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_F() {
    static short pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 0, 0, 0,
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_f() {
    static short pixels[4*7] = {
        0, 0, 0, 0,
        0, 0, 1, 0,
        0, 1, 0, 0,
        0, 1, 1, 0,
        0, 1, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 0
    };
    struct Character r = { .w = 4, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_G() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 0,
        0, 1, 0, 0, 0, 0,
        0, 1, 0, 1, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_g() {
    static short pixels[5*6] = {
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 0, 1, 0,
        0, 0, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 0, 0
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_H() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_h() {
    static short pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 0, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_I() {
    static short pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_i() {
    static short pixels[3*7] = {
        0, 0, 0,
        0, 1, 0,
        0, 0, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 0, 0,
    };
    struct Character r = { .w = 3, .h = 7, .pixels = pixels };
    return r;
}
struct Character char_J() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_j() {
    static short pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 0, 1, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_K() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 1, 0, 0,
        0, 1, 1, 0, 0, 0,
        0, 1, 0, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_k() {
    static short pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 0, 0,
        0, 1, 0, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_L() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_l() {
    static short pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 1, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_M() {
    static short pixels[7*7] = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 1, 0,
        0, 1, 1, 0, 1, 1, 0,
        0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 0, 0, 1, 0,
        0, 1, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 7, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_m() {
    static short pixels[7*6] = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 1, 0, 0,
        0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 7, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_N() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 0, 1, 0,
        0, 1, 0, 1, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_n() {
    static short pixels[5*6] = {
        0, 0, 0, 0, 0,
        0, 1, 1, 0, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_O() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_o() {
    static short pixels[6*6] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_P() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_p() {
    static short pixels[5*6] = {
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_Q() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 1, 0, 0,
        0, 0, 1, 0, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_q() {
    static short pixels[5*6] = {
        0, 0, 0, 0, 0, 
        0, 1, 1, 1, 0, 
        0, 1, 0, 1, 0, 
        0, 0, 1, 1, 0, 
        0, 0, 0, 1, 0, 
        0, 0, 0, 1, 0
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_R() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_r() {
    static short pixels[5*6] = {
        0, 0, 0, 0, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_S() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_s() {
    static short pixels[5*6] = {
        0, 0, 0, 0, 0, 
        0, 0, 1, 1, 0,
        0, 1, 1, 0, 0,
        0, 0, 1, 1, 0,
        0, 1, 1, 0, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_T() {
    static short pixels[7*7] = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
    };
    struct Character r = { .w = 7, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_t() {
    static short pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_U() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_u() {
    static short pixels[6*6] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_V() {
    static short pixels[7*7] = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 1, 0,
        0, 1, 0, 0, 0, 1, 0,
        0, 0, 1, 0, 1, 0, 0,
        0, 0, 1, 0, 1, 0, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 7, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_v() {
    static short pixels[7*6] = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 1, 0,
        0, 1, 0, 0, 0, 1, 0,
        0, 0, 1, 0, 1, 0, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 7, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_W() {
    static short pixels[7*7] = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0,
        0, 0, 1, 0, 1, 0, 0,
        0, 0, 1, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 7, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_w() {
    static short pixels[7*6] = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0,
        0, 0, 1, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 7, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_X() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0,
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_x() {
    static short pixels[7*6] = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 1, 1, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 1, 1, 0, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 7, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_Y() {
    static short pixels[7*7] = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 1, 0,
        0, 0, 1, 0, 1, 0, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 7, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_y() {
    static short pixels[7*6] = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 1, 0,
        0, 0, 1, 0, 1, 0, 0,
        0, 0, 1, 0, 1, 0, 0,
        0, 0, 0, 1, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0
    };
    struct Character r = { .w = 7, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_Z() {
    static short pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0,
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_z() {
    static short pixels[5*6] = {
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}
