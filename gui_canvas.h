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

#define CANVAS_UNIFORM_MVP_MATRIX 0
#define CANVAS_UNIFORM_NORMAL_MATRIX 1
#define CANVAS_UNIFORM_AMBIENT_COLOR 2
#define CANVAS_UNIFORM_DIFFUSE_COLOR 3

struct Canvas {
    struct CanvasComponents {
        int size;
        GLenum type;
        int bytes;
    } components[NUM_OGL_ATTRIBUTES];

    struct CanvasShader {
        char name[256];
        GLuint vertex_shader, fragment_shader, program;

        struct {
            GLint location;
            char name[256];
        } attribute[NUM_OGL_ATTRIBUTES];

        struct {
            GLint location;
            char name[256];
        } uniform[NUM_CANVAS_UNIFORMS];

    } shader[NUM_CANVAS_SHADER];

    struct CanvasLayer {
        // - so this mesh is just going to contain _everything_ for a layer, that means no individual meshes, but
        // all vertices for all meshes of a layer with transformed vertices and normals etc so that it can just
        // be rendered with the identity matrix as model_matrix and still comes out the way the user wants it
        //struct VboMesh* mesh;

        // - I don't want to bind the buffers in Vbo for every draw operation, so I cache the data in arrays here
        // and then upload it in one go before rendering, I keep the Vbo/VboMesh around so that I can just render
        // when there is nothing cached (?), well, to reuse it so I don't have to recreate the buffers every time
        // I render (?)
        struct CanvasAttributes {
            void* array;

            int capacity;
            int occupied;
        } attributes[NUM_OGL_ATTRIBUTES];

        struct CanvasIndices {
            unsigned int* array;

            int capacity;
            int occupied;
        } indices[NUM_CANVAS_SHADER][NUM_OGL_PRIMITIVES];

        // set this to > 0 if I want to map this layer to screen
        float screen;
    } layer[NUM_CANVAS_LAYER];
};

extern struct Canvas global_canvas;

int init_canvas();

void canvas_create(struct Canvas* canvas);
void canvas_add_attribute(struct Canvas* canvas, int attribute, int size, GLenum type, int bytes);

int canvas_append_shader_source(struct Canvas* canvas, const char* vertex_source, const char* fragment_source, const char* name);
int canvas_append_shader_program(struct Canvas* canvas, GLuint vertex_shader, GLuint fragment_shader, GLuint program, const char* name);
int canvas_find_shader(struct Canvas* canvas, const char* shader_name);

int canvas_alloc_attributes(struct Canvas* canvas, int layer_i, int attribute_i, int n);
int canvas_alloc_indices(struct Canvas* canvas, int layer_i, const char* shader_name, GLenum primitive_type, int n);

void canvas_clear(struct Canvas* canvas, int layer_start, int layer_end);

int canvas_append_vertices(struct Canvas* canvas, int layer_i, void* vertices, int n, const Mat model_matrix);
int canvas_append_colors(struct Canvas* canvas, int layer_i, void* colors, int n, const Color color);
int canvas_append_normals(struct Canvas* canvas, int layer_i, void* normals, int n);
int canvas_append_texcoords(struct Canvas* canvas, int layer_i, void* texcoords, int n);
int canvas_append_indices(struct Canvas* canvas, int layer_i, const char* shader_name, GLenum primitive_type, unsigned int* indices, int n, int offset);

#endif
