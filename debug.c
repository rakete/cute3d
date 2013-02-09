#include "debug.h"

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

/* GLuint debug_compile(const char *vertex_source, const char* fragment_source) { */
/*     GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER); */
/*     GLint vertex_length = strlen(vertex_source); */
/*     glShaderSource(vertex_shader, 1, &vertex_source, &vertex_length); */
/*     glCompileShader(vertex_shader); */
    
/*     GLint shader_ok; */
/*     glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_ok); */
/*     if ( ! shader_ok ) { */
/*         debug_info_log(vertex_shader, glGetShaderiv, glGetShaderInfoLog); */
/*         glDeleteShader(vertex_shader); */
/*         return 0; */
/*     } */
    
/*     GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER); */
/*     GLint fragment_length = strlen(fragment_source); */
/*     glShaderSource(fragment_shader, 1, &fragment_source, &fragment_length); */
/*     glCompileShader(fragment_shader); */
        
/*     glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_ok); */
/*     if ( ! shader_ok ) { */
/*         debug_info_log(fragment_shader, glGetShaderiv, glGetShaderInfoLog); */
/*         glDeleteShader(fragment_shader); */
/*         return 0; */
/*     } */
    
/*     GLuint program = glCreateProgram(); */
/*     glAttachShader(program, vertex_shader); */
/*     glAttachShader(program, fragment_shader); */
/*     glLinkProgram(program); */
    
/*     GLint program_ok; */
/*     glGetProgramiv(program, GL_LINK_STATUS, &program_ok); */
/*     if (!program_ok) { */
/*         debug_info_log(program, glGetProgramiv, glGetProgramInfoLog); */
/*         glDeleteProgram(program); */
/*         return 0; */
/*     } */
/*     return program; */
/* } */

GLuint debug_compile_source(GLenum type, const char* source, uint32_t length) {
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

GLuint debug_compile_file(GLenum type, const char* filename) {
    uint32_t length;
    GLchar* source = read_file(filename, &length);

    if(!source) return 0;

    fprintf(stderr, "Compiling: %s\n", filename);
    GLuint id = debug_compile_source(type, source, length);
    if( ! id ) {
        fprintf(stderr, "Compilation failed in: %s\n", filename);
    }
    return id;
}

GLuint debug_link_program(GLuint vertex_shader, GLuint fragment_shader) {
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

GLuint debug_make_program(const char *vertex_source, const char* fragment_source) {
    uint32_t length_vertex = strlen(vertex_source);
    uint32_t length_fragment = strlen(fragment_source);
    printf("length_vertex: %d\n", length_vertex);
    printf("length_fragment: %d\n", length_fragment);
    GLuint vertex = debug_compile_source(GL_VERTEX_SHADER, vertex_source, length_vertex);
    GLuint fragment = debug_compile_source(GL_FRAGMENT_SHADER, fragment_source, length_fragment);

    return debug_link_program(vertex, fragment);
}
