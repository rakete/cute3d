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

#ifndef NUM_CANVAS_TEXTS
#define NUM_CANVAS_TEXTS 2
#endif

struct Canvas {
    struct CanvasComponents {
        int32_t size;
        GLenum type;
        int32_t bytes;
    } components[NUM_SHADER_ATTRIBUTES];

    struct CanvasAttributes {
        void* array;
        int32_t capacity;
        int32_t occupied;
    } attributes[NUM_SHADER_ATTRIBUTES];

    struct CanvasBuffer {
        GLuint id;
        int32_t capacity;
        int32_t occupied;

        GLenum usage;
    } buffer[NUM_SHADER_ATTRIBUTES];

    struct Shader shader[NUM_CANVAS_SHADER];

    struct Font fonts[NUM_CANVAS_FONTS];

    // - so this mesh is just going to contain _everything_ for a layer, that means no individual meshes, but
    // all vertices for all meshes of a layer with transformed vertices and normals etc so that it can just
    // be rendered with the identity matrix as model_matrix and still comes out the way the user wants it
    //struct VboMesh* mesh;

    // - I don't want to bind the buffers in Vbo for every draw operation, so I cache the data in arrays here
    // and then upload it in one go before rendering, I keep the Vbo/VboMesh around so that I can just render
    // when there is nothing cached (?), well, to reuse it so I don't have to recreate the buffers every time
    // I render (?)
    struct CanvasLayer {
        struct CanvasIndices {
            uint32_t* array;
            GLuint id;

            int32_t capacity;
            int32_t occupied;
        } indices[NUM_CANVAS_SHADER][NUM_OGL_PRIMITIVES];

        struct CanvasText {
            uint32_t* array;
            GLuint id;

            int32_t capacity;
            int32_t occupied;
        } text[NUM_CANVAS_FONTS][NUM_CANVAS_TEXTS];

        //int32_t disable;
    } layer[NUM_CANVAS_LAYERS];
};

extern struct Canvas global_canvas;

int32_t init_canvas();

void canvas_create(struct Canvas* canvas);
void canvas_add_attribute(struct Canvas* canvas, int32_t attribute, int32_t size, GLenum type);

int32_t canvas_append_shader_source(struct Canvas* canvas, const char* vertex_source, const char* fragment_source, const char* name);
int32_t canvas_append_shader_program(struct Canvas* canvas, struct Shader* const shader, const char* name);
int32_t canvas_find_shader(struct Canvas* canvas, const char* shader_name);

int32_t canvas_append_font(struct Canvas* canvas, struct Font font, const char* font_name);
int32_t canvas_find_font(struct Canvas* canvas, const char* font_name);

int32_t canvas_alloc_attributes(struct Canvas* canvas, int32_t attribute_i, int32_t n);
int32_t canvas_alloc_indices(struct Canvas* canvas, int32_t layer_i, const char* shader_name, GLenum primitive_type, int32_t n);
int32_t canvas_alloc_text(struct Canvas* canvas, int32_t layer_i, int32_t text_i, const char* font_name, int32_t n);

void canvas_clear(struct Canvas* canvas, int32_t layer_start, int32_t layer_end);

int32_t canvas_append_vertices(struct Canvas* canvas, void* vertices, int32_t size, GLenum type, int32_t n, const Mat model_matrix);
int32_t canvas_append_colors(struct Canvas* canvas, void* colors, int32_t size, GLenum type, int32_t n, const Color color);
int32_t canvas_append_texcoords(struct Canvas* canvas, void* texcoords, int32_t size, GLenum type, int32_t n);
int32_t canvas_append_normals(struct Canvas* canvas, void* normals, int32_t size, GLenum type, int32_t n);

int32_t canvas_append_indices(struct Canvas* canvas, int32_t layer_i, const char* shader_name, GLenum primitive_type, uint32_t* indices, int32_t n, int32_t offset);
int32_t canvas_append_text(struct Canvas* canvas, int32_t layer_i, int32_t text_i, const char* font_name, uint32_t* indices, int32_t n, int32_t offset);

#endif
