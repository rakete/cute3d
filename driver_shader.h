#ifndef DRIVER_SHADER_H
#define DRIVER_SHADER_H

#include "stdio.h"

#include "math_matrix.h"
#include "driver_log.h"
#include "driver_ogl.h"
#include "driver_sdl2.h"
#include "driver_glsl.h"
#include "driver_shader.h"

// sooner or later I am going to need to supply custom attributes to shaders,
// defining them here like this seems to be the most sane way
#define MAX_CUSTOM_ATTRIBUTES 10
#define MAX_SHADER_ATTRIBUTES MAX_CUSTOM_ATTRIBUTES+MAX_OGL_ATTRIBUTES

// attribute ids for arrays
#define SHADER_ATTRIBUTE_VERTEX OGL_VERTEX
#define SHADER_ATTRIBUTE_VERTEX_TEXCOORD OGL_TEXCOORD
#define SHADER_ATTRIBUTE_VERTEX_NORMAL OGL_NORMAL
#define SHADER_ATTRIBUTE_VERTEX_COLOR OGL_COLOR

#define SHADER_ATTRIBUTE_DIFFUSE_COLOR MAX_OGL_ATTRIBUTES+0
#define SHADER_ATTRIBUTE_SPECULAR_COLOR MAX_OGL_ATTRIBUTES+1
#define SHADER_ATTRIBUTE_AMBIENT_COLOR MAX_OGL_ATTRIBUTES+2
#define SHADER_ATTRIBUTE_HARD_NORMAL MAX_OGL_ATTRIBUTES+3
#define SHADER_ATTRIBUTE_SMOOTH_NORMAL MAX_OGL_ATTRIBUTES+4
#define SHADER_ATTRIBUTE_INSTANCE_ID MAX_OGL_ATTRIBUTES+5
#define SHADER_ATTRIBUTE_PREV_VERTEX MAX_OGL_ATTRIBUTES+6
#define SHADER_ATTRIBUTE_NEXT_VERTEX MAX_OGL_ATTRIBUTES+7
#define SHADER_ATTRIBUTE_LINE_THICKNESS MAX_OGL_ATTRIBUTES+8
#define SHADER_ATTRIBUTE_BARYCENTRIC_COORDINATE MAX_OGL_ATTRIBUTES+9

// uniform ids for arrays (and maybe locations)
#define MAX_SHADER_UNIFORMS 17
#define SHADER_UNIFORM_MVP_MATRIX 0
#define SHADER_UNIFORM_MODEL_MATRIX 1
#define SHADER_UNIFORM_VIEW_MATRIX 2
#define SHADER_UNIFORM_PROJECTION_MATRIX 3
#define SHADER_UNIFORM_NORMAL_MATRIX 4

#define SHADER_UNIFORM_DIFFUSE_LIGHT 5
#define SHADER_UNIFORM_SPECULAR_LIGHT 6
#define SHADER_UNIFORM_AMBIENT_LIGHT 7
#define SHADER_UNIFORM_LIGHT_DIRECTION 8
#define SHADER_UNIFORM_LIGHT_POSITION 9
#define SHADER_UNIFORM_LIGHT_ATTENUATION 10
#define SHADER_UNIFORM_MATERIAL_SHININESS 11
#define SHADER_UNIFORM_MATERIAL_COEFFICIENTS 12
#define SHADER_UNIFORM_EYE_POSITION 13

#define SHADER_UNIFORM_ASPECT_RATIO 14
#define SHADER_UNIFORM_LINE_Z_SCALING 15
#define SHADER_UNIFORM_ENABLE_TEXTURE 16


// - samplers are just uniforms, but they behave different so I treat them different
// - essentially a sampler  just contains a number, which is the active texture unit that is sampled
// - I restrict myself to max 8 active texture units, which should be supported by all opengl
// implementations
// - to associate shader samplers (like uniform sampler2D diffuse_texture in a shader) with a texture
// unit (the thing that I have to call glActiveTexture on), I have these defines like
// SHADER_SAMPLER_DIFFUSE_TEXTURE
// - I want those to be fixed, that is I want SHADER_SAMPLER_DIFFUSE_TEXTURE with value 0 to be always
// associated with texture unit 0, that way I can setup a shaders sampler once (with the value 0 if it
// is the sampler for the diffuse texture) and then not worry about having to set them again while
// rendering, reducing the amount of state changes
// - but since I only have 8 (depending on the hardware) available texture units, I would run into
// problems as soon as I want to have more the 8 different samplers, I need to reuse some texture units,
// I can't directly map from SHADER_SAMPLER_DIFFUSE_TEXTURE=0 -> GL_TEXTURE0
// - to do that I want to organize these defines in blocks of 8, where each block is associated with 8
// unique texture units, so then while I need unique texture units for every sampler of each block, I
// can reuse texture units in different blocks
// - for example the diffuse texture is associated with texture unit 0 in the first block, but then I
// want a shader which takes diffuse atlas _instead_ of a diffuse texture, then I can reuse texture
// unit 0 for the diffuse atlas sampler by putting it at the first spot in a new block, like I've done
// below by giving SHADER_SAMPLER_DIFFUSE_ATLAS the value 8
#define MAX_SHADER_TEXTURE_UNITS 8
#define MAX_SHADER_SAMPLER 16
#define SHADER_SAMPLER_DIFFUSE_TEXTURE 0
#define SHADER_SAMPLER_DIFFUSE_ATLAS 8

// names for locations
extern const char* global_shader_attribute_names[MAX_SHADER_ATTRIBUTES];
extern const char* global_shader_uniform_names[MAX_SHADER_UNIFORMS];
extern const char* global_shader_sampler_names[MAX_SHADER_SAMPLER];

struct Shader {
    char name[256];
    GLuint vertex_shader, fragment_shader, program;

    // - instead of using glGetAttribLocation in shader_add_attribute calls it may be better
    // to use fixed locations for the attributes and then use glBindAttribLocation and always
    // bind to the same location
    // - but I don't because I have to use glGetUniformLocation anyways and I like that both cases
    // are handled in a similar fashion
    // - I could use (layout = 0) qualifiers but opengl 3.1 only has those for attributes and not
    // uniforms and opengl es2 has them not at all (I think)
    struct {
        GLint location;
        char name[256];
        bool unset;
        bool warn_once;
    } attribute[MAX_SHADER_ATTRIBUTES];

    // - I was thinking about using uniform buffer objects and how they would fit in here,
    // the verdict was that instead of locations from glGetUniformLocation I could store
    // offsets from glGetActiveUniformsiv here, or I could use the std140 layout for every
    // shader which would give me fixed offsets making the calls to query the offsets
    // uneccessary
    // - shader_add_uniform would need to make these new calls, for that it can get a new string
    // argument which indicates the block it should query, so it could get its index with
    // glGetUniformBlockIndex, maybe I would have to store that index as well
    // - I decided against it and kept using the standard glUniform... calls because there
    // are quite a lot of hits for "uniform buffer objects performance" where peoply are having
    // problems with them
    // - ubo are not supported in opengl es2
    // - there are implementation dependent limitations that would have to be checked and handled
    // and I just don't think the added complexity is worth it
    // - debugging is twice as hard as writing code, so if you write your code as smart as you can,
    // you will not be smart enough to debug it
    struct {
        GLint location;
        char name[256];
        bool unset;
        bool warn_once;
    } uniform[MAX_SHADER_UNIFORMS];

    struct {
        GLint location;
        char name[256];
        bool unset;
        bool warn_once;
    } sampler[MAX_SHADER_SAMPLER];

    bool verified;
};

WARN_UNUSED_RESULT int32_t init_shader();

void shader_create(struct Shader* p);
void shader_create_from_files(const char* vertex_file, const char* fragment_file, const char* name, struct Shader* p);
void shader_create_from_sources(const char* vertex_source, const char* fragment_source, const char* name, struct Shader* p);

void shader_use_program(const struct Shader* p);

void shader_setup_locations(struct Shader* p);
void shader_verify_locations(struct Shader* p);
void shader_warn_locations(struct Shader* p, GLint* attribute_locations);

GLint shader_add_attribute(struct Shader* shader, int32_t attribute_index, const char* name);
GLint shader_add_uniform(struct Shader* shader, int32_t uniform_index, const char* name);
GLint shader_add_sampler(struct Shader* shader, int32_t sampler_index, const char* name);

void shader_print(FILE* f, const struct Shader* shader);

GLint shader_set_uniform_matrices(struct Shader* shader, GLuint program, const Mat projection_matrix, const Mat view_matrix, const Mat model_matrix);

GLint shader_set_uniform_1f(struct Shader* shader, GLuint program, int32_t uniform_index, uint32_t size, GLenum type, void* data);
GLint shader_set_uniform_1i(struct Shader* shader, GLuint program, int32_t uniform_index, uint32_t size, GLenum type, void* data);

GLint shader_set_uniform_3f(struct Shader* shader, GLuint program, int32_t uniform_index, uint32_t size, GLenum type, void* data);
GLint shader_set_uniform_4f(struct Shader* shader, GLuint program, int32_t uniform_index, uint32_t size, GLenum type, void* data);

GLint shader_set_attribute(struct Shader* shader, int32_t attribute_i, GLuint buffer, GLint c_num, GLenum c_type, GLsizei stride, const GLvoid* p);

#endif
