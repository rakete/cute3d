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

#ifndef MAX_FONT_GLYPHS
#define MAX_FONT_GLYPHS 256
#endif

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
    char name[256];

    // a glyph is a texture point plus width and height for every character that can be displayed
    // with this font
    struct Glyph glyphs[MAX_FONT_GLYPHS];

    // the glyphs will only be initialized if the alphabet string given to font_create contains it,
    // so this array can be used to check if a character can be displayed with this font, if its
    // glyph is initialized
    bool alphabet[MAX_FONT_GLYPHS];

    bool unicode;

    struct {
        GLuint id;
        size_t width;
        size_t height;
        GLenum type;
        int32_t format;
        int32_t min_filter;
        int32_t mag_filter;
    } texture;

    struct Shader shader;

    float kerning;
    float linespacing;
};

void font_create(const wchar_t* alphabet, bool unicode, struct Character* symbols, const char* name, struct Font* font);

void font_texture_filter(struct Font* font, GLint min_filter, GLint mag_filter);

#endif
