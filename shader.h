#ifndef SHADER_H
#define SHADER_H

#include "stdio.h"

#include "GL/glew.h"
#include "GL/gl.h"

#include "glsl.h"

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

struct Shader {
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

struct Shader* shader_create(struct Shader* p, const char* vertex_file, const char* fragment_file);
void shader_attribute(struct Shader* shader, int array_id, char* name);
void shader_uniform(struct Shader* shader, char* name, char* type, void* data);

#endif
