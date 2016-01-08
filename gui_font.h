#ifndef GUI_FONT_H
#define GUI_FONT_H

#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "stdbool.h"
#include "string.h"
#include "wchar.h"

#include "math_types.h"
#include "driver_ogl.h"
#include "driver_glsl.h"
#include "driver_shader.h"

#ifndef NUM_FONT_GLYPHS
#define NUM_FONT_GLYPHS 1024
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
    char name[256];
    struct Glyph glyphs[NUM_FONT_GLYPHS];
    bool alphabet[NUM_FONT_GLYPHS];

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

    struct Shader shader;

    float kerning;
    float linespacing;
    Color color;
};

void font_create(struct Font* font, const wchar_t* alphabet, bool unicode, struct Character* symbols, const char* name);

void font_texture_filter(struct Font* font, GLint min_filter, GLint mag_filter);

#endif
