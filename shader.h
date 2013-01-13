#ifndef SHADER_H
#define SHADER_H

#include "stdio.h"

#include "GL/glew.h"
#include "GL/gl.h"

#include "io.h"
#include "debug.h"

#ifndef SHADER_UNIFORMS
#define SHADER_UNIFORMS 32
#endif

#ifndef SHADER_ATTRIBUTES
#define SHADER_ATTRIBUTES 3
#endif

/* enum uniform_type { */
/*     invalid_uniform = 0, */
/*     uniform1f, */
/*     uniform2f, */
/*     uniform3f, */
/*     uniform4f */
/* }; */

struct shader {
    GLuint vertex_shader, fragment_shader, program;

    uint32_t active_uniforms;
    struct {
        char name[256];
    } uniform[SHADER_UNIFORMS];
    
    struct {
        char name[256];
    } attribute[SHADER_ATTRIBUTES];
};

int init_shader();

GLuint compile_shader(GLenum type, const char *filename);
GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);

struct shader* shader_create();
void shader_attribute(struct shader* shader, int array_id, char* name);
void shader_uniform1f(struct shader* shader, char* name, float value);

#endif
