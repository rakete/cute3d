#include "glsl.h"

void debug_info_log( GLuint object,
                     PFNGLGETSHADERIVPROC glGet__iv,
                     PFNGLGETSHADERINFOLOGPROC glGet__InfoLog )
{
    GLint log_length;
    char *log;

    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
    log = malloc(log_length);
    glGet__InfoLog(object, log_length, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
}

GLuint compile_source(GLenum type, const char* source, uint32_t length) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, &length);
    glCompileShader(shader);

    GLint shader_ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if ( ! shader_ok ) {
        fprintf(stderr, "Failed to compile: %s\n", source);
        debug_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint compile_file(GLenum type, const char* filename) {
    uint32_t length;
    GLchar* source = read_file(filename, &length);

    if(!source) return 0;

    fprintf(stderr, "Compiling: %s\n", filename);
    GLuint id = compile_source(type, source, length);
    if( ! id ) {
        fprintf(stderr, "Compilation failed in: %s\n", filename);
    }
    return id;
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
        debug_info_log(program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

GLuint make_program(const char *vertex_source, const char* fragment_source) {
    uint32_t length_vertex = strlen(vertex_source);
    uint32_t length_fragment = strlen(fragment_source);
    printf("length_vertex: %d\n", length_vertex);
    printf("length_fragment: %d\n", length_fragment);
    GLuint vertex = compile_source(GL_VERTEX_SHADER, vertex_source, length_vertex);
    GLuint fragment = compile_source(GL_FRAGMENT_SHADER, fragment_source, length_fragment);

    return link_program(vertex, fragment);
}
