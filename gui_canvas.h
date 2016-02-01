#ifndef GUI_CANVAS_H
#define GUI_CANVAS_H

#include "string.h"

#include "driver_ogl.h"
#include "driver_shader.h"
#include "driver_log.h"
#include "math_matrix.h"

#include "gui_font.h"
#include "gui_default_font.h"

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

// - I created this so I could create functions that draw by stuff by filling arrays with transformed
// vertices, and then render those all at once in a mainloop
// - this behaves somewhat similar to the vbo stuff I implemented
struct Canvas {
    // information about size and type the vertex/normal/color/texcoord attribute components
    // that are stored in the arrays/vbos
    struct CanvasComponents {
        uint32_t size;
        GLenum type;
        uint32_t bytes;
    } components[NUM_SHADER_ATTRIBUTES];

    // array pointers that get allocated with malloc and filled, one per attribute
    struct CanvasAttributes {
        void* array;
        size_t capacity;
        size_t occupied;
    } attribute[NUM_SHADER_ATTRIBUTES];

    // - vbos that are supposed to be filled by an external render function, one per attribute
    // - these are one big buffer that are supposed to be bound once before rendering all the
    // stuff on the canvas
    struct CanvasAttributeBuffer {
        GLuint id;
        size_t capacity;
        size_t occupied;

        GLenum usage;
    } buffer[NUM_SHADER_ATTRIBUTES];

    // - shaders can be appended too, I put this here because I basicly give up all means
    // to distinguish 'stuff' that I want to draw, once I appended it into an array, so
    // to be able to use different shaders, I need to be able to distinguish attributes
    // which belong to different shaders
    // - adding them like this makes it more convenient to implement render functions, I
    // probably could also have solved this with an argument give to render functions
    struct Shader shader[NUM_CANVAS_SHADER];

    // - I decided to make text rendering a special case and put fonts in here too,
    // a font is just a special shader with some extra info, so maybe this could have
    // been solved with just shaders
    // - it is easier like this though, and text rendering is such a common use case
    // that I prefer the clarity of this approach over a more compact solution,
    // I also did not want to redo the font creation stuff in here
    // - I'll probably have to add more special cases anyways, widget for example
    // come to mind
    struct Font fonts[NUM_CANVAS_FONTS];

    // - the arrays and vbos above only hold vertex data, the indices are kept in these
    // structs, the struct are indexed by all the things that are needed to distinguish
    // stuff for rendering
    // - layers are at the top level and mostly convenience to easly group stuff when
    // drawing on the canvas, so I can selectivly render
    // - the indices struct is indexed so that different shaders, projections and primitives
    // are possible options when drawing
    // - text can have different fonts or projections, to put text directly on the screen
    // or display in the world
    struct CanvasLayer {
        struct CanvasIndices {
            GLuint* array;
            GLuint id;

            size_t capacity;
            size_t occupied;
        } indices[NUM_CANVAS_SHADER][NUM_CANVAS_PROJECTIONS][NUM_OGL_PRIMITIVES];

        struct CanvasText {
            GLuint* array;
            GLuint id;

            size_t capacity;
            size_t occupied;
        } text[NUM_CANVAS_FONTS][NUM_CANVAS_PROJECTIONS];

        Vec4f cursor;
    } layer[NUM_CANVAS_LAYERS];
};

extern struct Canvas global_canvas;

int32_t init_canvas() __attribute__((warn_unused_result));

void canvas_create(struct Canvas* canvas);
void canvas_create_default(struct Canvas* canvas);

// - the _add_ in general just initialise settings or something like, they are not neccessary to be
// called every frame, just once after creating the canvas
// - in particular the _add_attribute initializes one of the components[attribute] structs to contain
// information about size and type of the attribute components
// - the _add_shader and _add_font function initialize shader fonts so that they can be used in some
// external rendering functions, these basicly just copy the settings of the given struct into the
// internal structs
// - the _find_ functions look through the arrays of struct for a name, then return the index of the
// struct with that name
void canvas_add_attribute(struct Canvas* canvas, int32_t attribute, uint32_t size, GLenum type);

int32_t canvas_add_shader(struct Canvas* canvas, const struct Shader* shader);
int32_t canvas_find_shader(struct Canvas* canvas, const char* shader_name);

int32_t canvas_add_font(struct Canvas* canvas, const struct Font* font);
int32_t canvas_find_font(struct Canvas* canvas, const char* font_name);

// - allocating the heap memory for the arrays
size_t canvas_alloc_attributes(struct Canvas* canvas, int32_t attribute_i, size_t n);
size_t canvas_alloc_indices(struct Canvas* canvas, int32_t layer_i, int32_t projection_i, const char* shader_name, GLenum primitive_type, size_t n);
size_t canvas_alloc_text(struct Canvas* canvas, int32_t layer_i, int32_t projection_i, const char* font_name, size_t n);

// - clear is supposed to be called every frame and completely resets all occupied counters, but leaves
// the allocated memory, so that memory once allocated is reused in repeating draw calls, that should
// restrict the calls to malloc to the very beginning of the program
void canvas_clear(struct Canvas* canvas, int32_t layer_start, int32_t layer_end);

// - the _append_ functions are used to fill the arrays with data, they always append to the end of the already occupied space
// - these check the allocated capacity and call the alloc functions if there is not enough space available for the new data
// - append_vertices and append_colors take one extra argument:
//   the append_vertices takes an optional model_matrix that should be applied to the vertices before appending
size_t canvas_append_vertices(struct Canvas* canvas, void* vertices, uint32_t size, GLenum type, size_t n, const Mat model_matrix);
//   the append_colors takes an optional color that can be used instead of an array consisting entirely of just one color
size_t canvas_append_colors(struct Canvas* canvas, void* colors, uint32_t size, GLenum type, size_t n, const Color color);
size_t canvas_append_texcoords(struct Canvas* canvas, void* texcoords, uint32_t size, GLenum type, size_t n);
size_t canvas_append_normals(struct Canvas* canvas, void* normals, uint32_t size, GLenum type, size_t n);

// - the append functions for the indices, they take all neccessary arguments to distinguish drawn stuff for rendering,
// e.g. what to render with what shader, what projection, etc.
// - then the functions also takes an offset that is to be added to every index before appending
size_t canvas_append_indices(struct Canvas* canvas, int32_t layer_i, int32_t projection_i, const char* shader_name, GLenum primitive_type, uint32_t* indices, size_t n, size_t offset);
size_t canvas_append_text(struct Canvas* canvas, int32_t layer_i, int32_t projection_i, const char* font_name, uint32_t* indices, size_t n, size_t offset);

#endif
