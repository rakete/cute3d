/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2ii013 Andreas Raster */

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

#include "gui_default_font.h"

// palette: default_font_palette
// format: rgb
uint8_t global_default_font_palette[1*9*3] = {
    0,   0,   0,
    255, 255, 255,
    0,   0,   0,
    255,   0,   0,
    0, 255,   0,
    0,   0, 255,
    255, 255,   0,
    0, 255, 255,
    230,   0, 150
};

struct Character char_A() {
    // bitmap: char_A
    // using: default_font_palette
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[6*6] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*7] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*6] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*7] = {
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
    static int32_t pixels[5*7] = {
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
    static int32_t pixels[5*6] = {
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
    static int32_t pixels[5*7] = {
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
    static int32_t pixels[4*7] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*6] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*7] = {
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
    static int32_t pixels[5*7] = {
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
    static int32_t pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0, 0,
        0, 1, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}
struct Character char_J() {
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*7] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*7] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*7] = {
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
    static int32_t pixels[7*7] = {
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
    static int32_t pixels[7*6] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*6] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[6*6] = {
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
    static int32_t pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_p() {
    static int32_t pixels[5*6] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*6] = {
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0
    };
    struct Character r = { .w = 5, .h = 6, .pixels = pixels };
    return r;
}

struct Character char_R() {
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*6] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*6] = {
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
    static int32_t pixels[7*7] = {
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
    static int32_t pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_U() {
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[6*6] = {
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
    static int32_t pixels[7*7] = {
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
    static int32_t pixels[7*6] = {
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
    static int32_t pixels[7*7] = {
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
    static int32_t pixels[7*6] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[7*6] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[6*6] = {
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
    static int32_t pixels[6*7] = {
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
    static int32_t pixels[5*6] = {
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

struct Character char_0() {
    static int32_t pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 1, 1, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_1() {
    static int32_t pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_2() {
    static int32_t pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_3() {
    static int32_t pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_4() {
    static int32_t pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_5() {
    static int32_t pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_6() {
    static int32_t pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_7() {
    static int32_t pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_8() {
    static int32_t pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_9() {
    static int32_t pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_period() {
    static int32_t pixels[4*7] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 0
    };
    struct Character r = { .w = 4, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_comma() {
    static int32_t pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}


struct Character char_colon() {
    static int32_t pixels[4*7] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 0
    };
    struct Character r = { .w = 4, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_semicolon() {
    static int32_t pixels[4*7] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 0,
        0, 1, 0, 0,
        0, 1, 0, 0
    };
    struct Character r = { .w = 4, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_plus() {
    static int32_t pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_minus() {
    static int32_t pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_equal() {
    static int32_t pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_asterisk() {
    static int32_t pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 1, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 1, 0, 1, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_slash() {
    static int32_t pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_open_paren() {
    static int32_t pixels[5*7] = {
        0, 0, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_close_paren() {
    static int32_t pixels[5*7] = {
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_open_bracket() {
    static int32_t pixels[5*7] = {
        0, 0, 1, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 1, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_close_bracket() {
    static int32_t pixels[5*7] = {
        0, 1, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_open_brace() {
    static int32_t pixels[5*7] = {
        0, 0, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_close_brace() {
    static int32_t pixels[5*7] = {
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

void default_font_create(struct Character symbols[256]) {
    symbols['A'] = char_A();
    symbols['B'] = char_B();
    symbols['C'] = char_C();
    symbols['D'] = char_D();
    symbols['E'] = char_E();
    symbols['F'] = char_F();
    symbols['G'] = char_G();
    symbols['H'] = char_H();
    symbols['I'] = char_I();
    symbols['J'] = char_J();
    symbols['K'] = char_K();
    symbols['L'] = char_L();
    symbols['M'] = char_M();
    symbols['N'] = char_N();
    symbols['O'] = char_O();
    symbols['P'] = char_P();
    symbols['Q'] = char_Q();
    symbols['R'] = char_R();
    symbols['S'] = char_S();
    symbols['T'] = char_T();
    symbols['U'] = char_U();
    symbols['V'] = char_V();
    symbols['W'] = char_W();
    symbols['X'] = char_X();
    symbols['Y'] = char_Y();
    symbols['Z'] = char_Z();

    symbols['a'] = char_a();
    symbols['b'] = char_b();
    symbols['c'] = char_c();
    symbols['d'] = char_d();
    symbols['e'] = char_e();
    symbols['f'] = char_f();
    symbols['g'] = char_g();
    symbols['h'] = char_h();
    symbols['i'] = char_i();
    symbols['j'] = char_j();
    symbols['k'] = char_k();
    symbols['l'] = char_l();
    symbols['m'] = char_m();
    symbols['n'] = char_n();
    symbols['o'] = char_o();
    symbols['p'] = char_p();
    symbols['q'] = char_q();
    symbols['r'] = char_r();
    symbols['s'] = char_s();
    symbols['t'] = char_t();
    symbols['u'] = char_u();
    symbols['v'] = char_v();
    symbols['w'] = char_w();
    symbols['x'] = char_x();
    symbols['y'] = char_y();
    symbols['z'] = char_z();

    symbols['0'] = char_0();
    symbols['1'] = char_1();
    symbols['2'] = char_2();
    symbols['3'] = char_3();
    symbols['4'] = char_4();
    symbols['5'] = char_5();
    symbols['6'] = char_6();
    symbols['7'] = char_7();
    symbols['8'] = char_8();
    symbols['9'] = char_9();

    symbols['.'] = char_period();
    symbols[','] = char_comma();
    symbols[':'] = char_colon();
    symbols[';'] = char_semicolon();

    symbols['+'] = char_plus();
    symbols['-'] = char_minus();
    symbols['*'] = char_asterisk();
    symbols['/'] = char_slash();
    symbols['='] = char_equal();

    symbols['('] = char_open_paren();
    symbols[')'] = char_close_paren();
    symbols['['] = char_open_bracket();
    symbols[']'] = char_close_bracket();
    symbols['{'] = char_open_brace();
    symbols['}'] = char_close_brace();
}
