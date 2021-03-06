/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef GUI_FONT_H
#define GUI_FONT_H

#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "stdbool.h"
#include "string.h"
#include "wchar.h"

#include "driver_ogl.h"
#include "driver_glsl.h"

#include "math_types.h"

#include "render_shader.h"

#ifndef MAX_FONT_GLYPHS
#define MAX_FONT_GLYPHS 256
#endif

#define MAX_FONT_ALPHABET_SIZE 65535

struct Character {
    int32_t w;
    int32_t h;
    const int32_t* pixels;
};

struct Glyph {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
};

struct Font {
    // - a glyph is a texture point plus width and height for every character that can be displayed
    // with this font
    struct Glyph glyphs[MAX_FONT_GLYPHS];

    // - the glyphs will only be initialized if the alphabet string given to font_create contains it,
    // so this array can be used to check if a character can be displayed with this font, if its
    // glyph is initialized
    // - this was the same size as glyphs and had type bool, now its wchar_t (could be int, but why
    // waste space, its never going to be > WCHAR_MAX) and has its own size so that I can store the
    // position of the glyph here, indexed by a wchar, before _both_ alphabet and glyphs were indexed
    // by char, which made using unicode impossible
    wchar_t alphabet[MAX_FONT_ALPHABET_SIZE];

    struct {
        GLuint id;
        size_t width;
        size_t height;
    } texture;

    struct Shader shader;

    float kerning;
    float linespacing;
};

void font_create(struct Font* font);
void font_create_from_characters(const wchar_t* unicode_alphabet, size_t symbols_n, struct Character* symbols, size_t palette_n, size_t color_n, uint8_t* palette, struct Font* font);

#endif
