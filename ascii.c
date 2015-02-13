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

#include "ascii.h"

#define SYMBOLS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

// palette: bnw
// format: rgb
static float palette[1*8*3] = {
    0.0, 0.0, 0.0,
    1.0, 1.0, 1.0,
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 0.0,
    0.0, 1.0, 1.0,
    0.9, 0.0, 0.6
};


struct Character char_A() {
    // bitmap: char_A
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_a
    // using: bnw
    static int pixels[6*6] = {
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
    // bitmap: char_B
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_b
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_C
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_c
    // using: bnw
    static int pixels[5*6] = {
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
    // bitmap: char_D
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_d
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_E
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_e
    // using: bnw
    static int pixels[5*6] = {
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
    // bitmap: char_F
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_f
    // using: bnw
    static int pixels[4*7] = {
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
    // bitmap: char_G
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_g
    // using: bnw
    static int pixels[5*6] = {
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
    // bitmap: char_H
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_h
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_I
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_i
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_J
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_j
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_K
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_k
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_L
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_l
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_M
    // using: bnw
    static int pixels[7*7] = {
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
    // bitmap: char_m
    // using: bnw
    static int pixels[7*6] = {
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
    // bitmap: char_N
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_n
    // using: bnw
    static int pixels[5*6] = {
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
    // bitmap: char_O
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_o
    // using: bnw
    static int pixels[6*6] = {
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
    // bitmap: char_P
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_p
    // using: bnw
    static int pixels[5*6] = {
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
    // bitmap: char_Q
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_q
    // using: bnw
    static int pixels[5*6] = {
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
    // bitmap: char_R
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_r
    // using: bnw
    static int pixels[5*6] = {
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
    // bitmap: char_S
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_s
    // using: bnw
    static int pixels[5*6] = {
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
    // bitmap: char_T
    // using: bnw
    static int pixels[7*7] = {
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
    // bitmap: char_t
    // using: bnw
    static int pixels[5*7] = {
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
    // bitmap: char_U
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_u
    // using: bnw
    static int pixels[6*6] = {
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
    // bitmap: char_V
    // using: bnw
    static int pixels[7*7] = {
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
    // bitmap: char_v
    // using: bnw
    static int pixels[7*6] = {
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
    // bitmap: char_W
    // using: bnw
    static int pixels[7*7] = {
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
    // bitmap: char_w
    // using: bnw
    static int pixels[7*6] = {
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
    // bitmap: char_X
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_x
    // using: bnw
    static int pixels[7*6] = {
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
    // bitmap: char_Y
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_y
    // using: bnw
    static int pixels[6*6] = {
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
    // bitmap: char_Z
    // using: bnw
    static int pixels[6*7] = {
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
    // bitmap: char_z
    // using: bnw
    static int pixels[5*6] = {
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
    static int pixels[6*7] = {
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
    static int pixels[5*7] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 1, 0,
        0, 0, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 0, 0
    };
    struct Character r = { .w = 5, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_2() {
    static int pixels[6*7] = {
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
    static int pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 0, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_4() {
    static int pixels[6*7] = {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 1, 0, 1, 0,
        0, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0
    };
    struct Character r = { .w = 6, .h = 7, .pixels = pixels };
    return r;
}

struct Character char_5() {
    static int pixels[6*7] = {
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
    static int pixels[6*7] = {
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
    static int pixels[6*7] = {
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
    static int pixels[6*7] = {
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
    static int pixels[6*7] = {
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
    static int pixels[4*7] = {
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
    static int pixels[4*7] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 1, 0, 0,
        0, 1, 0, 0
    };
    struct Character r = { .w = 4, .h = 7, .pixels = pixels };
    return r;
}


struct Character char_colon() {
    static int pixels[4*7] = {
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
    static int pixels[4*7] = {
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

void ascii_create(struct Character symbols[256]) {
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
}
