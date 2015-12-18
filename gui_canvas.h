#ifndef GUI_CANVAS_H
#define GUI_CANVAS_H

#include "string.h"

#include "driver_ogl.h"
#include "math_matrix.h"

#ifndef DEFAULT_CANVAS_ALLOC
#define DEFAULT_CANVAS_ALLOC 512
#endif

#ifndef NUM_CANVAS_LAYER
#define NUM_CANVAS_LAYER 32
#endif

struct Canvas {
    struct CanvasComponents {
        int size;
        GLenum type;
        int bytes;
    } components[NUM_OGL_ATTRIBUTES];

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
            void* array[NUM_OGL_ATTRIBUTES];

            int capacity;
            int occupied;
        } attributes;

        struct CanvasIndices {
            unsigned int* array;

            int capacity;
            int occupied;
        } indices[NUM_OGL_PRIMITIVES];

        // set this to > 0 if I want to map this layer to screen
        float screen;
    } layer[NUM_CANVAS_LAYER];
};

extern struct Canvas global_canvas;

int init_canvas();

void canvas_create(struct Canvas* canvas);
void canvas_add_attributes(struct Canvas* canvas, int attribute, int size, GLenum type, int bytes);

int canvas_alloc_attributes(struct Canvas* canvas, int layer_i, int n);
int canvas_alloc_indices(struct Canvas* canvas, int layer_i, int n, GLenum primitive_type);

void canvas_clear(struct Canvas* canvas, int layer_start, int layer_end);

void canvas_append_attributes(struct Canvas* canvas, int layer_i, void* vertices, void* colors, void* normals, void* texcoords, int n);
void canvas_append_indices(struct Canvas* canvas, int layer_i, int* indices, int n, GLenum primitive_type);

#endif
