#ifndef GUI_CANVAS_H
#define GUI_CANVAS_H

#include "string.h"

#include "driver_ogl.h"
#include "driver_shader.h"
#include "driver_log.h"
#include "math_matrix.h"

#include "gui_font.h"

#ifndef DEFAULT_CANVAS_ALLOC
#define DEFAULT_CANVAS_ALLOC 512
#endif

#ifndef NUM_CANVAS_LAYERS
#define NUM_CANVAS_LAYERS 32
#endif

#ifndef NUM_CANVAS_SHADER
#define NUM_CANVAS_SHADER 8
#endif

#ifndef NUM_CANVAS_UNIFORMS
#define NUM_CANVAS_UNIFORMS 16
#endif

#ifndef NUM_CANVAS_FONTS
#define NUM_CANVAS_FONTS 8
#endif

#ifndef NUM_CANVAS_PROJECTIONS
#define NUM_CANVAS_PROJECTIONS 2
#endif

#define CANVAS_PROJECT_WORLD 0
#define CANVAS_PROJECT_SCREEN 1

struct Canvas {
    struct CanvasComponents {
        uint32_t size;
        GLenum type;
        uint32_t bytes;
    } components[NUM_SHADER_ATTRIBUTES];

    struct CanvasAttributes {
        void* array;
        size_t capacity;
        size_t occupied;
    } attributes[NUM_SHADER_ATTRIBUTES];

    struct CanvasBuffer {
        GLuint id;
        size_t capacity;
        size_t occupied;

        GLenum usage;
    } buffer[NUM_SHADER_ATTRIBUTES];

    struct Shader shader[NUM_CANVAS_SHADER];

    struct Font fonts[NUM_CANVAS_FONTS];

    struct CanvasLayer {
        struct CanvasIndices {
            GLuint* array;
            GLuint id;

            size_t capacity;
            size_t occupied;
        } indices[NUM_CANVAS_SHADER][NUM_OGL_PRIMITIVES];

        struct CanvasText {
            GLuint* array;
            GLuint id;

            size_t capacity;
            size_t occupied;
        } text[NUM_CANVAS_FONTS][NUM_CANVAS_PROJECTIONS];

        Vec4f cursor;

        //int32_t disable;
    } layer[NUM_CANVAS_LAYERS];
};

extern struct Canvas global_canvas;

int32_t init_canvas() __attribute__((warn_unused_result));

void canvas_create(struct Canvas* canvas);
void canvas_add_attribute(struct Canvas* canvas, int32_t attribute, uint32_t size, GLenum type);

int32_t canvas_append_shader(struct Canvas* canvas, struct Shader* const shader, const char* name);
int32_t canvas_find_shader(struct Canvas* canvas, const char* shader_name);

int32_t canvas_append_font(struct Canvas* canvas, struct Font font, const char* font_name);
int32_t canvas_find_font(struct Canvas* canvas, const char* font_name);

size_t canvas_alloc_attributes(struct Canvas* canvas, int32_t attribute_i, size_t n);
size_t canvas_alloc_indices(struct Canvas* canvas, int32_t layer_i, const char* shader_name, GLenum primitive_type, size_t n);
size_t canvas_alloc_text(struct Canvas* canvas, int32_t layer_i, int32_t text_i, const char* font_name, size_t n);

void canvas_clear(struct Canvas* canvas, int32_t layer_start, int32_t layer_end);

size_t canvas_append_vertices(struct Canvas* canvas, void* vertices, uint32_t size, GLenum type, size_t n, const Mat model_matrix);
size_t canvas_append_colors(struct Canvas* canvas, void* colors, uint32_t size, GLenum type, size_t n, const Color color);
size_t canvas_append_texcoords(struct Canvas* canvas, void* texcoords, uint32_t size, GLenum type, size_t n);
size_t canvas_append_normals(struct Canvas* canvas, void* normals, uint32_t size, GLenum type, size_t n);

size_t canvas_append_indices(struct Canvas* canvas, int32_t layer_i, const char* shader_name, GLenum primitive_type, uint32_t* indices, size_t n, size_t offset);

size_t canvas_append_text(struct Canvas* canvas, int32_t layer_i, int32_t text_i, const char* font_name, uint32_t* indices, size_t n, size_t offset);

#endif
