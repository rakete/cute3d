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

#ifndef GUI_DEFAULT_FONT_H
#define GUI_DEFAULT_FONT_H

#include "gui_font.h"

struct Character char_A();
struct Character char_B();
struct Character char_C();
struct Character char_D();
struct Character char_E();
struct Character char_F();
struct Character char_G();
struct Character char_H();
struct Character char_I();
struct Character char_J();
struct Character char_K();
struct Character char_L();
struct Character char_M();
struct Character char_N();
struct Character char_O();
struct Character char_P();
struct Character char_Q();
struct Character char_R();
struct Character char_S();
struct Character char_T();
struct Character char_U();
struct Character char_V();
struct Character char_W();
struct Character char_X();
struct Character char_Y();
struct Character char_Z();

struct Character char_a();
struct Character char_b();
struct Character char_c();
struct Character char_d();
struct Character char_e();
struct Character char_f();
struct Character char_g();
struct Character char_h();
struct Character char_i();
struct Character char_j();
struct Character char_k();
struct Character char_l();
struct Character char_m();
struct Character char_n();
struct Character char_o();
struct Character char_p();
struct Character char_q();
struct Character char_r();
struct Character char_s();
struct Character char_t();
struct Character char_u();
struct Character char_v();
struct Character char_w();
struct Character char_x();
struct Character char_y();
struct Character char_z();

struct Character char_0();
struct Character char_1();
struct Character char_2();
struct Character char_3();
struct Character char_4();
struct Character char_5();
struct Character char_6();
struct Character char_7();
struct Character char_8();
struct Character char_9();

struct Character char_period();
struct Character char_comma();
struct Character char_colon();
struct Character char_semicolon();

struct Character char_plus();
struct Character char_minus();
struct Character char_asterisk();
struct Character char_slash();
struct Character char_equal();

struct Character char_open_paren();
struct Character char_close_paren();
struct Character char_open_bracket();
struct Character char_close_bracket();
struct Character char_open_brace();
struct Character char_close_brace();

void default_font_create(struct Character symbols[256]);

#endif
