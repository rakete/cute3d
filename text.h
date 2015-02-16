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

#ifndef TEXT_H
#define TEXT_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "wchar.h"

#include "ogl.h"
#include "glsl.h"
#include "math_types.h"
#include "ascii.h"

#include "camera.h"

#ifndef FONT_SIZE
#define FONT_SIZE 1024
#endif

struct Glyph {
    int x;
    int y;
    int w;
    int h;
};

struct Font {
    struct Glyph glyphs[FONT_SIZE];
    bool alphabet[FONT_SIZE];

    bool unicode;

    struct {
        GLuint id;
        GLsizei width;
        GLsizei height;
        GLenum type;
        GLint format;
        GLint min_filter;
        GLint mag_filter;
    } texture;

    struct {
        GLuint program;
    } shader;

    float kerning;
    float linespacing;
    Color color;
};

void font_create(const wchar_t* alphabet, bool unicode, struct Character* symbols, struct Font* font);

void font_texture_filter(struct Font* font, GLint min_filter, GLint mag_filter);

void text_put(const wchar_t* text, const struct Font* font, float scale, const Mat projection_matrix, const Mat view_matrix, Mat model_matrix);
void text_overlay(const wchar_t* text, const struct Font* font, int size, struct Camera camera, int x, int y);

#endif
