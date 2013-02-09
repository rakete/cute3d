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
        short* alphabet;
        size_t size;
    } heap;

    struct {
        short unicode;
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

enum FontOp {
    NewFont = 0,
    CloneFont,
    FindFont,
    DeleteFont
};

int font_registry(enum FontOp op, int id, struct Font** font);

/* int font_id = font_registry(NewFont, 0, &(font_allocate_ascii("abcd", symbols)) ); */
/* struct Font* font; */
/* if( font_registry(FindFont, font_id, &font) ) { */
/*     printf("%f\n", font->kerning); */
/* } */
/* font_registry(DeleteFont, font_id, NULL); */

struct Font* font_allocate_ascii(const char* alphabet, struct Character* symbols);
struct Font* font_allocate_utf(const wchar_t* alphabet, struct Character* symbols);
void font_delete(struct Font* font);

void font_texture_filter(struct Font* font, GLint min_filter, GLint mag_filter);

void text_render(const wchar_t* text, const struct Font* font, const Matrix projection_matrix, const Matrix view_matrix, const Matrix model_matrix);
void text_screen(const wchar_t* text, const struct Font* font, const Matrix projection_matrix, const Matrix view_matrix, int x, int y);

#endif
