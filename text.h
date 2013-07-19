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

#include "GL/glew.h"
#include "GL/gl.h"

#include "glsl.h"
#include "math_types.h"
#include "ascii.h"

#ifndef NUM_FONTS
#define NUM_FONTS 32
#endif

struct Glyph {
    int x;
    int y;
    int w;
    int h;
};

struct Font {
    struct {
        struct Glyph* glyphs;
        bool* alphabet;
        size_t size;
    } heap;

    struct {
        bool unicode;
        size_t size;
    } encoding;
    
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
    float size;
    Color color;
};

enum FontReg {
    NewFont = 0,
    CloneFont,
    FindFont,
    DeleteFont
};

int font_registry(enum FontReg op, int id, struct Font** font);

struct Font* font_allocate_ascii(const char* alphabet, struct Character* symbols);
struct Font* font_allocate_utf(const wchar_t* alphabet, struct Character* symbols);
void font_delete(struct Font* font);

void font_texture_filter(struct Font* font, GLint min_filter, GLint mag_filter);

void text_put(const wchar_t* text, const struct Font* font, const Matrix projection_matrix, const Matrix view_matrix, const Matrix model_matrix);
void text_putscreen(const wchar_t* text, const struct Font* font, const Matrix projection_matrix, const Matrix view_matrix, int x, int y);

#endif
