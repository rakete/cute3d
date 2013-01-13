#include "shader.h"

int init_shader() {
    int ret = 1;

    glewInit();
    if( ! glewGetExtension("GL_ARB_uniform_buffer_object") )
    {
        printf("ERROR: uniform_buffer_object extension not found!\n");
        ret = 0;
    }

    return ret;
}

GLuint compile_shader(GLenum type, const char *filename) {
    GLint length;
    GLchar* source = read_file(filename, &length);

    if(!source) return 0;

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, &length);
    free(source);
    glCompileShader(shader);

    GLint shader_ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if ( ! shader_ok ) {
        fprintf(stderr, "Failed to compile %s:\n", filename);
        show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint link_program(GLuint vertex_shader, GLuint fragment_shader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint program_ok;
    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        fprintf(stderr, "Failed to link shader program:\n");
        show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

struct shader* shader_create(struct shader* p) {
    p->vertex_shader = compile_shader(GL_VERTEX_SHADER, "default.vertex");
    p->fragment_shader = compile_shader(GL_FRAGMENT_SHADER, "default.fragment");

    if( p->vertex_shader > 0 && p->fragment_shader > 0 ) {
        p->program = link_program(p->vertex_shader, p->fragment_shader);
    } else {
        p->program = 0;
    }

    p->active_uniforms = 0;
    for( int i = 0; i < SHADER_UNIFORMS; i++ ) {
        strncpy(p->uniform[i].name, "\0", 1);
    }

    for( int i = 0; i < SHADER_ATTRIBUTES; i++ ) {
        strncpy(p->attribute[i].name, "\0", 1);
    }
}

void shader_attribute(struct shader* shader, int array_id, char* name) {
    if( strlen(name) < 256 ) {
        strncpy(shader->attribute[array_id].name, name, strlen(name)+1);
    }
}

void shader_uniform1f(struct shader* shader, char* name, float value) {
    if( shader->active_uniforms < SHADER_UNIFORMS &&
        strlen(name) < 256 )
    {
        glUseProgram(shader->program);
        
        strncpy(shader->uniform[shader->active_uniforms].name, name, strlen(name));

        GLint id = glGetUniformLocation(shader->program, name);
        glUniform1f(id, value);
        
        shader->active_uniforms++;
    }
}
