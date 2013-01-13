#include "debug.h"

void show_info_log( GLuint object,
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

void debug_grid(float projection_matrix[16], float view_matrix[16], float model_matrix[16], int steps) {
    static GLuint grid = 0;
    static GLuint program = 0;
    
    const char* vertex_source =
        "#version 130\n"
        "#extension GL_ARB_uniform_buffer_object:require\n"
        "\n"
        "uniform mat4 projection_matrix;\n"
        "uniform mat4 model_matrix;\n"
        "uniform mat4 view_matrix;\n"
        "\n"
        "in vec3 vertex;\n"
        "uniform vec4 color;\n"
        "\n"
        "smooth out vec4 frag_color;\n"
        "\n"
        "void main() {\n"
        "    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex,1.0);\n"
        "    frag_color = color;\n"
        "}";

    const char* fragment_source =
        "#version 130\n"
        "\n"
        "smooth in vec4 frag_color;\n"
        "\n"
        "void main() {\n"
        "    gl_FragColor = frag_color;\n"
        "}";
    
    if( ! program ) {
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        GLint vertex_length = strlen(vertex_source);
        glShaderSource(vertex_shader, 1, &vertex_source, &vertex_length);
        glCompileShader(vertex_shader);
        
        GLint shader_ok;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_ok);
        if ( ! shader_ok ) {
            show_info_log(vertex_shader, glGetShaderiv, glGetShaderInfoLog);
            glDeleteShader(vertex_shader);
        }
        
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        GLint fragment_length = strlen(fragment_source);
        glShaderSource(fragment_shader, 1, &fragment_source, &fragment_length);
        glCompileShader(fragment_shader);
        
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_ok);
        if ( ! shader_ok ) {
            show_info_log(fragment_shader, glGetShaderiv, glGetShaderInfoLog);
            glDeleteShader(fragment_shader);
        }

        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        
        GLint program_ok;
        glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
        if (!program_ok) {
            show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
            glDeleteProgram(program);
        }
    }
    
    float w = 1.0f;
    float h = 1.0f;
    int size = (steps+1)*2 + (steps+1)*2;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;
    if( ! grid && program ) {
        // 1  5  9 10----11
        // |  |  |
        // |  |  | 6-----7
        // |  |  |
        // 0  4  8 2-----3
        GLfloat vertices[size * 3];
        GLuint elements[size];

        for( int i = 0; i < (steps+1); i++ ) {
            float xf = -w/2.0f + (float)i * (w / (float)steps);

            vertices[i * 12 + 0]  = xf;
            vertices[i * 12 + 1]  = -h/2.0f;
            vertices[i * 12 + 2]  = 0.0;

            elements[i * 4 + 0]   = i * 4 + 0;
                     
            vertices[i * 12 + 3]  = xf;
            vertices[i * 12 + 4]  = h/2.0f ;
            vertices[i * 12 + 5]  = 0.0f;

            elements[i * 4 + 1]   = i * 4 + 1;

            float yf = -h/2.0f + i * (h / (float)steps);
            
            vertices[i * 12 + 6]  = -w/2.0f;
            vertices[i * 12 + 7]  = yf;
            vertices[i * 12 + 8]  = 0.0;
        
            elements[i * 4 + 2]   = i * 4 + 2;
        
            vertices[i * 12 + 9]  = w/2.0f;
            vertices[i * 12 + 10] = yf;
            vertices[i * 12 + 11] = 0.0f;
        
            elements[i * 4 + 3]   = i * 4 + 3;
        }
        
        glGenVertexArrays(1, &grid);
        glBindVertexArray(grid);
        
        glGenBuffers(1, &vertices_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

        glGenBuffers(1, &elements_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);       

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    if( grid && program ) {
        glBindVertexArray(grid);

        glUseProgram(program);

        GLint projection_loc = glGetUniformLocation(program, "projection_matrix");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);
        
        GLint model_loc = glGetUniformLocation(program, "model_matrix");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix);

        GLint view_loc = glGetUniformLocation(program, "view_matrix");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);

        GLint color_loc = glGetUniformLocation(program, "color");
        glUniform4f(color_loc, 0.0f, 0.0f, 1.0f, 1.0f);

        //glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
        
        // vao does not bind element array on intel with mesa
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);

        glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);

        glUseProgram(0);
        glBindVertexArray(0);
    }
}

