#ifndef DRIVER_SHADER_H
#define DRIVER_SHADER_H

#include "stdio.h"

#include "math_camera.h"
#include "driver_log.h"
#include "driver_ogl.h"
#include "driver_glsl.h"
#include "driver_shader.h"

// names for locations
#define SHADER_NAME_VERTICES "vertex"
#define SHADER_NAME_NORMALS "normal"
#define SHADER_NAME_COLORS "color"
#define SHADER_NAME_TEXCOORDS "texcoord"
#define SHADER_NAME_MVP_MATRIX "mvp_matrix"
#define SHADER_NAME_MODEL_MATRIX "model_matrix"
#define SHADER_NAME_VIEW_MATRIX "view_matrix"
#define SHADER_NAME_PROJECTION_MATRIX "projection_matrix"
#define SHADER_NAME_NORMAL_MATRIX "normal_matrix"
#define SHADER_NAME_LIGHT_DIRECTION "light_direction"
#define SHADER_NAME_AMBIENT_COLOR "ambient_color"
#define SHADER_NAME_DIFFUSE_COLOR "diffuse_color"
#define SHADER_NAME_DIFFUSE_TEXTURE "diffuse_texture"

// uniform ids for arrays (and maybe locations)
#define NUM_SHADER_UNIFORMS 32
#define SHADER_UNIFORM_MVP_MATRIX 0
#define SHADER_UNIFORM_MODEL_MATRIX 1
#define SHADER_UNIFORM_VIEW_MATRIX 2
#define SHADER_UNIFORM_PROJECTION_MATRIX 3
#define SHADER_UNIFORM_NORMAL_MATRIX 4
#define SHADER_UNIFORM_LIGHT_DIRECTION 5
#define SHADER_UNIFORM_AMBIENT_COLOR 6
#define SHADER_UNIFORM_DIFFUSE_COLOR 7

// attribute ids for arrays (and maybe locations)
#define NUM_SHADER_ATTRIBUTES 4
#define SHADER_ATTRIBUTE_VERTICES OGL_VERTICES
#define SHADER_ATTRIBUTE_NORMALS OGL_NORMALS
#define SHADER_ATTRIBUTE_COLORS OGL_COLORS
#define SHADER_ATTRIBUTE_TEXCOORDS OGL_TEXCOORDS
/* SHADER_ATTRIBUTE_INTERLEAVED_VNCT, */
/* SHADER_ATTRIBUTE_INTERLEAVED_NCT, */
/* SHADER_ATTRIBUTE_INTERLEAVED_VCT, */
/* SHADER_ATTRIBUTE_INTERLEAVED_VNT, */
/* SHADER_ATTRIBUTE_INTERLEAVED_VNC, */
/* SHADER_ATTRIBUTE_INTERLEAVED_VN, */
/* SHADER_ATTRIBUTE_INTERLEAVED_VC, */
/* SHADER_ATTRIBUTE_INTERLEAVED_VT, */
/* SHADER_ATTRIBUTE_INTERLEAVED_NC, */
/* SHADER_ATTRIBUTE_INTERLEAVED_NT, */
/* SHADER_ATTRIBUTE_INTERLEAVED_CT, */

struct Shader {
    char name[256];
    GLuint vertex_shader, fragment_shader, program;

    // - instead of using glGetAttribLocation in shader_add_attribute calls it may be better
    // to use fixed locations for the attributes and then use glBindAttribLocation and always
    // bind to the same location
    // - but I don't because I have to use glGetUniformLocation anyways and I like that both cases
    // are handled in a similar fashion
    // - I could use (layout = 0) qualifiers but opengl 3.1 only has those for attributes and not
    // uniforms and opengl es 2 has them not at all (I think)
    struct {
        GLint location;
        char name[256];
    } attribute[NUM_SHADER_ATTRIBUTES];

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
    // - ubo are not supported in opengl es 2
    // - there are implementation dependent limitations that would have to be checked and handled
    // and I just don't think the added complexity is worth it
    // - debugging is twice as hard as writing code, so if you write your code as smart as you can,
    // you will not be smart enough to debug it
    struct {
        GLint location;
        char name[256];
    } uniform[NUM_SHADER_UNIFORMS];
};

int32_t init_shader() __attribute__((warn_unused_result));

void shader_create_empty(struct Shader* p);
void shader_create_from_files(const char* vertex_file, const char* fragment_file, const char* name, struct Shader* p);
void shader_create_from_sources(const char* vertex_source, const char* fragment_source, const char* name, struct Shader* p);
void shader_copy(struct Shader* const src, struct Shader* dst);

GLint shader_add_attribute(struct Shader* shader, int32_t attribute_index, const char* name);
GLint shader_add_uniform(struct Shader* shader, int32_t uniform_index, const char* name, const char* type, void* data);

void shader_create_flat(const char* name, struct Shader* shader);
void shader_create_gl_lines(const char* name, struct Shader* shader);

void shader_print(FILE* f, struct Shader* const shader);

void shader_matrices(struct Shader* const shader, struct Camera* const camera, Mat const model_matrix);

#endif
