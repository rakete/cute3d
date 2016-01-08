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
#define SHADER_ATTRIBUTE_VERTICES 0
#define SHADER_ATTRIBUTE_NORMALS 1
#define SHADER_ATTRIBUTE_COLORS 2
#define SHADER_ATTRIBUTE_TEXCOORDS 3
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

    struct {
        GLint location;
        char name[256];
    } attribute[NUM_SHADER_ATTRIBUTES];

    struct {
        GLint location;
        char name[256];
    } uniform[NUM_SHADER_UNIFORMS];
};

int init_shader();

void shader_create_empty(struct Shader* p);
void shader_create_from_files(const char* vertex_file, const char* fragment_file, const char* name, struct Shader* p);
void shader_create_from_sources(const char* vertex_source, const char* fragment_source, const char* name, struct Shader* p);
void shader_copy(struct Shader* const src, struct Shader* dst);

GLint shader_attribute(struct Shader* shader, int attribute_index, const char* name, int n, const char* type, void* data);
GLint shader_uniform(struct Shader* shader, int uniform_index, const char* name, const char* type, void* data);

void shader_create_flat(const char* name, struct Shader* shader);
void shader_create_gl_lines(const char* name, struct Shader* shader);

void shader_print(FILE* f, struct Shader* const shader);

void shader_matrices(struct Shader* const shader, struct Camera* const camera, Mat const model_matrix);

#endif
