#ifndef FONT_H
#define FONT_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "GL/glew.h"
#include "GL/gl.h"

#include "ascii.h"

struct Glyph {
    int x;
    int y;
    int w;
    int h;
};

typedef struct Character SymbolTable[256];

struct Font {
    struct Glyph glyphs[256];
    struct {
        GLuint id;
        GLsizei width;
        GLsizei height;
        GLenum type;
        GLint format;
        GLint min_filter;
        GLint mag_filter;
    } texture;
};

void font_create(struct Font* font, const char* alphabet, SymbolTable symbols);

void font_texture_filter(struct Font* font, GLint min_filter, GLint mag_filter);

#endif
