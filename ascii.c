/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "ascii.h"

struct Character char_A() {
    static bool pixels[6*7] = {
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
    static bool pixels[6*6] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*7] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*6] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*7] = {
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
    static bool pixels[5*7] = {
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
    static bool pixels[5*6] = {
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 0, 0, 0,
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_F() {
    static bool pixels[5*7] = {
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
    static bool pixels[4*7] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*6] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*7] = {
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
    static bool pixels[5*7] = {
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
    static bool pixels[3*7] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*7] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*7] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*7] = {
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
    static bool pixels[7*7] = {
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
    static bool pixels[7*6] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*6] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[6*6] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*6] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*6] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*6] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[5*6] = {
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
    static bool pixels[7*7] = {
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
    static bool pixels[5*7] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[6*6] = {
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
    static bool pixels[7*7] = {
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
    static bool pixels[7*6] = {
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
    static bool pixels[7*7] = {
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
    static bool pixels[7*6] = {
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
    static bool pixels[6*7] = {
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
    static bool pixels[7*6] = {
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
    static bool pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0,
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_y() {
    static bool pixels[6*6] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0
    };
    struct Character r = { .w = 6, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_Z() {
    static bool pixels[6*7] = {
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
    static bool pixels[5*6] = {
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
