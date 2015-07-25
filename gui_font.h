#ifndef GUI_FONT_H
#define GUI_FONT_H

#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "stdbool.h"
#include "string.h"
#include "wchar.h"

#include "math_types.h"
#include "render_ogl.h"
#include "render_glsl.h"

#ifndef FONT_SIZE
#define FONT_SIZE 1024
#endif

struct Character {
    int w;
    int h;
    const int* pixels;
};

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

#endif
