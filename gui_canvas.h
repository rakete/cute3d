#ifndef GUI_CANVAS_H
#define GUI_CANVAS_H

#include "string.h"

#include "driver_ogl.h"
#include "driver_log.h"
#include "driver_texture.h"

#include "math_matrix.h"

#include "gui_font.h"
#include "gui_default_font.h"

#ifndef DEFAULT_CANVAS_ALLOC
#define DEFAULT_CANVAS_ALLOC 512
#endif

#ifndef MAX_CANVAS_LAYERS
#define MAX_CANVAS_LAYERS 3
#endif

#ifndef MAX_CANVAS_SHADER
#define MAX_CANVAS_SHADER 4
#endif

#ifndef MAX_CANVAS_FONTS
#define MAX_CANVAS_FONTS 2
#endif

#ifndef MAX_CANVAS_PROJECTIONS
#define MAX_CANVAS_PROJECTIONS 2
#endif

#define CANVAS_PROJECT_WORLD 0
#define CANVAS_PROJECT_SCREEN 1

#ifndef MAX_CANVAS_TEXTURES
#define MAX_CANVAS_TEXTURES 8
#endif

#ifndef MAX_CANVAS_PRIMITIVES
#define MAX_CANVAS_PRIMITIVES 2
#endif

#define CANVAS_TRIANGLES 0
#define CANVAS_LINES 1

#define CANVAS_NO_TEXTURE MAX_CANVAS_TEXTURES

// - I created this so I could create functions that draw by stuff by filling arrays with transformed
// vertices, and then render those all at once in a mainloop
// - this behaves somewhat similar to the vbo stuff I implemented
struct Canvas {
    char name[256];

    int32_t width;
    int32_t height;

    // information about size and type the vertex/normal/color/texcoord attribute components
    // that are stored in the arrays/vbos
    struct CanvasComponents {
        uint32_t size;
        GLenum type;
        uint32_t bytes;
    } components[MAX_SHADER_ATTRIBUTES];

    // array pointers that get allocated with malloc and filled, one per attribute
    struct CanvasAttributes {
        void* array;
        size_t capacity;
        size_t occupied;
    } attributes[MAX_SHADER_ATTRIBUTES];

    // - vbos that are supposed to be filled by an external render function, one per attribute
    // - these are one big buffer that are supposed to be bound once before rendering all the
    // stuff on the canvas
    struct CanvasAttributeBuffer {
        GLuint id;
        size_t capacity;
        size_t occupied;

        GLenum usage;
    } buffer[MAX_SHADER_ATTRIBUTES];

    // - shaders can be appended too, I put this here because I basicly give up all means
    // to distinguish 'stuff' that I want to draw, once I appended it into an array, so
    // to be able to use different shaders, I need to be able to distinguish attributes
    // which belong to different shaders
    // - adding them like this makes it more convenient to implement render functions, I
    // probably could also have solved this with an argument given to render the functions
    struct CanvasShader {
        struct Shader shader;
        char name[256];
    } shaders[MAX_CANVAS_SHADER];

    // - I decided to make text rendering a special case and put fonts in here too,
    // a font is just a special shader with some extra info, so maybe this could have
    // been solved with just shaders
    // - it is easier like this though, and text rendering is such a common use case
    // that I prefer the clarity of this approach over a more compact solution,
    // I also did not want to redo the font creation stuff in here
    // - I'll probably have to add more special cases anyways, widget for example
    // come to mind
    struct CanvasFonts {
        struct Font font;
        char name[256];
    } fonts[MAX_CANVAS_FONTS];

    struct CanvasTextures {
        struct Texture sampler[MAX_SHADER_SAMPLER];
        char name[256];
    } textures[MAX_CANVAS_TEXTURES];

    // - the arrays and vbos above only hold vertex data, the indices are kept in these
    // structs, the struct are indexed by all the things that are needed to distinguish
    // stuff for rendering, they can't be in a single large buffer because I want to append
    // indices in arbitrary order without worrying about chunk sizes
    // - layers are at the top level and mostly convenience to easly group stuff when
    // drawing on the canvas, so I can selectivly render
    // - the indices struct is indexed so that different shaders, projections and primitives
    // are possible options when drawing
    // - text can have different fonts or projections, to put text directly on the screen
    // or display in the world
    // - I added the MAX_CANVAS_TEXTURES+1 dim to the indices array so that I can different
    // textures, it needs to be +1 so I can have one index representing no texture CANVAS_NO_TEXTURE
    struct CanvasLayer {
        struct CanvasIndices {
            GLuint* array;
            GLuint id;

            size_t capacity;
            size_t occupied;
        } indices[MAX_CANVAS_TEXTURES+1][MAX_CANVAS_SHADER][MAX_CANVAS_PROJECTIONS][MAX_CANVAS_PRIMITIVES];

        struct CanvasText {
            GLuint* array;
            GLuint id;

            size_t capacity;
            size_t occupied;
        } text[MAX_CANVAS_FONTS][MAX_CANVAS_PROJECTIONS];

        Vec4f cursor;
    } layer[MAX_CANVAS_LAYERS];

#ifndef CUTE_BUILD_ES2
    GLuint vao;
#endif

    float line_z_scaling;
};

extern struct Canvas global_dynamic_canvas;
extern struct Canvas global_static_canvas;

WARN_UNUSED_RESULT int32_t init_canvas(int32_t width, int32_t height);

void canvas_create(const char* name, int32_t width, int32_t height, struct Canvas* canvas);
// - I use malloc for the arrays, I should implement a destructor eventually, but since the canvas
// is meant to be present throughout the whole runtime of the program this is not a priority
void canvas_destroy(struct Canvas* canvas);

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

WARN_UNUSED_RESULT int32_t canvas_add_shader(struct Canvas* canvas, const char* shader_name, const struct Shader* shader);
WARN_UNUSED_RESULT int32_t canvas_find_shader(const struct Canvas* canvas, const char* shader_name);

WARN_UNUSED_RESULT int32_t canvas_add_font(struct Canvas* canvas, const char* font_name, const struct Font* font);
WARN_UNUSED_RESULT int32_t canvas_find_font(const struct Canvas* canvas, const char* font_name);

WARN_UNUSED_RESULT int32_t canvas_add_texture(struct Canvas* canvas, int32_t sampler, const char* texture_name, const struct Texture* texture);
WARN_UNUSED_RESULT int32_t canvas_find_texture(const struct Canvas* canvas, const char* texture_name);

// - allocating the heap memory for the arrays
size_t canvas_alloc_attributes(struct Canvas* canvas, uint32_t attribute_i, size_t n);
size_t canvas_alloc_indices(struct Canvas* canvas, int32_t layer_i, int32_t texture_i, const char* shader_name, int32_t projection_i, GLenum primitive_type, size_t n);
size_t canvas_alloc_text(struct Canvas* canvas, int32_t layer_i, const char* font_name, int32_t projection_i, size_t n);

// - clear is supposed to be called every frame and completely resets all occupied counters, but leaves
// the allocated memory, so that memory once allocated is reused in repeating draw calls, that should
// restrict the calls to malloc to the very beginning of the program
// - well, the way I coded the canvas, one huge attribute buffer for all layers, it does not make sense
// to clear it only partly, because I always have to clear all attributes which makes having the indices
// still around useless, so now this function always clears all layers
void canvas_clear(struct Canvas* canvas);

void canvas_clear_shaders(struct Canvas* canvas);
void canvas_clear_fonts(struct Canvas* canvas);
void canvas_clear_textures(struct Canvas* canvas);

// - the _append_ functions are used to fill the arrays with data, they always append to the end of the already occupied space
// - these check the allocated capacity and call the alloc functions if there is not enough space available for the new data
size_t canvas_append_attributes(struct Canvas* canvas, uint32_t attribute_i, uint32_t size, GLenum type, size_t n, void* attributes);

// - the append functions for the indices, they take all neccessary arguments to distinguish drawn stuff for rendering,
// e.g. what to render with what shader, what projection, etc.
// - then the functions also takes an offset that is to be added to every index before appending
size_t canvas_append_indices(struct Canvas* canvas, int32_t layer_i, int32_t texture_i, const char* shader_name, int32_t projection_i, GLenum primitive_type, size_t n, uint32_t* indices, size_t offset);
size_t canvas_append_text(struct Canvas* canvas, int32_t layer_i, const char* font_name, int32_t projection_i, size_t n, uint32_t* indices, size_t offset);

#endif
