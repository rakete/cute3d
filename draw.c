#include "draw.h"

void draw_grid( int instances,
                 int steps,
                 float color[4],
                 float projection_matrix[16],
                 float view_matrix[16],
                 float model_matrix[][16] )
{    
    const char* vertex_source =
        GLSL( uniform mat4 projection_matrix;
              uniform mat4 model_matrix;
              uniform mat4 view_matrix;
              in vec3 vertex;
              uniform vec4 color;
              out vec4 frag_color;
              void main() {
                  gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex,1.0);
                  frag_color = color;
              });

    const char* fragment_source =
        GLSL( in vec4 frag_color;
              void main() {
                  gl_FragColor = frag_color;
              });

    static GLuint program = 0;
    if( ! program ) {
        program = debug_make_program(vertex_source, fragment_source);
    }

    static GLuint grid[0xffff] = {};
    float w = 1.0f;
    float h = 1.0f;
    int size = (steps+1)*2 + (steps+1)*2;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;
    GLfloat vertices[size * 3];
    GLuint elements[size];
    if( ! grid[steps] && program ) {
        // 1  5  9 10----11
        // |  |  |
        // |  |  | 6-----7
        // |  |  |
        // 0  4  8 2-----3
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
        
        glGenVertexArrays(1, &grid[steps]);
        glBindVertexArray(grid[steps]);
        
        glGenBuffers(1, &vertices_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

        glGenBuffers(1, &elements_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);       
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if( grid[steps] && program ) {
        glBindVertexArray(grid[steps]);

        glUseProgram(program);

        GLint color_loc = glGetUniformLocation(program, "color");
        glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

        GLint projection_loc = glGetUniformLocation(program, "projection_matrix");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);
        
        GLint view_loc = glGetUniformLocation(program, "view_matrix");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);

        for( int i = 0; i < instances; i++ ) {
            GLint model_loc = glGetUniformLocation(program, "model_matrix");
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix[i]);

            glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);
        }

        glUseProgram(0);
        glBindVertexArray(0);
    }
}

void draw_normals_array( float* vertices,
                          float* normals,
                          int n,
                          float color[4],
                          float projection_matrix[16],
                          float view_matrix[16],
                          float model_matrix[16] )
{    
    const char* vertex_source =
        GLSL( uniform mat4 projection_matrix;
              uniform mat4 model_matrix;
              uniform mat4 view_matrix;
              in vec3 vertex;
              uniform vec4 color;
              out vec4 frag_color;
              void main() {
                  gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex,1.0);
                  frag_color = color;
              });

    const char* fragment_source =
        GLSL( in vec4 frag_color;
              void main() {
                  gl_FragColor = frag_color;
              });

    static GLuint program = 0;
    if( ! program ) {
        program = debug_make_program(vertex_source, fragment_source);
    }

    static GLuint arrow = 0;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;
    GLfloat arrow_vertices[18] =
        { 0.0,  0.0,  0.0,
          0.0,  0.0,  0.5,
          0.05,  0.0,  0.4,
          0.0,  0.05,  0.4,
          -0.05, 0.0,  0.4,
          0.0,  -0.05, 0.4 };
        
    GLuint arrow_elements[18] =
        { 0, 1,
          1, 2,
          1, 3,
          1, 4,
          1, 5,
          2, 3,
          3, 4,
          4, 5,
          5, 2 };

    if( ! arrow ) {
        glGenVertexArrays(1, &arrow);
        glBindVertexArray(arrow);
        
        glGenBuffers(1, &vertices_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(arrow_vertices), arrow_vertices, GL_STATIC_DRAW);

        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

        glGenBuffers(1, &elements_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);       
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrow_elements), arrow_elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    if( arrow && program ) {
        glBindVertexArray(arrow);

        glUseProgram(program);

        GLint color_loc = glGetUniformLocation(program, "color");
        glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

        GLint projection_loc = glGetUniformLocation(program, "projection_matrix");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);
        
        GLint view_loc = glGetUniformLocation(program, "view_matrix");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);


        for( int i = 0; i < n; i++ ) {
            Matrix arrow_matrix;
            matrix_identity(arrow_matrix);
            
            Vec z = { 0.0, 0.0, 1.0, 1.0 };
            Vec normal = { normals[i*3+0], normals[i*3+1], normals[i*3+2] };
            Vec axis;
            vector_cross(normal,z,axis);
            if( vnullp(axis) ) {
                vector_perpendicular(z,axis);
            }
            
            float angle;
            vector_angle(normal,z,&angle);
            
            Quat rotation;
            rotation_quat(axis,angle,rotation);
            quat_matrix(rotation, arrow_matrix, arrow_matrix);

            Vec vertex = { vertices[i*3+0], vertices[i*3+1], vertices[i*3+2], 1.0 };
            matrix_translate(arrow_matrix, vertex, arrow_matrix);

            matrix_multiply(arrow_matrix, model_matrix, arrow_matrix);
                        
            GLint model_loc = glGetUniformLocation(program, "model_matrix");
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, arrow_matrix);

            glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);
        }

        glUseProgram(0);
        glBindVertexArray(0);
    }

}

void draw_texture_quad( GLuint texture_id,
                         float projection_matrix[16],
                         float view_matrix[16],
                         float model_matrix[16] )
{
    const char* vertex_source =
        GLSL( uniform mat4 projection_matrix;
              uniform mat4 model_matrix;
              uniform mat4 view_matrix;
              in vec3 vertex;
              in vec2 texcoord;
              uniform vec3 normal;
              uniform vec4 color;
              out vec4 frag_color;
              out vec2 frag_texcoord;
              void main() {
                  gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex,1.0);
                  frag_color = color;
                  frag_texcoord = texcoord;
              });

    const char* fragment_source =
        GLSL( uniform sampler2D diffuse;
              uniform ivec2 offset;
              uniform ivec2 glyph;
              in vec4 frag_color;
              in vec2 frag_texcoord;
              void main() {
                  gl_FragColor = texture(diffuse, frag_texcoord);
              });

    static GLuint program = 0;
    if( ! program ) {
        program = debug_make_program(vertex_source, fragment_source);
    }

    static GLuint quad = 0;
    static GLuint vertices_id = 0;
    static GLuint texcoords_id = 0;
    static GLuint elements_id = 0;
    GLfloat vertices[6*3] =
        { -1.0, 1.0, 0.0,
          1.0, 1.0, 0.0,
          1.0, -1.0, 0.0,
          1.0, -1.0, 0.0,
          -1.0, -1.0, 0.0,
          -1.0, 1.0, 0.0 };
    GLfloat texcoords[6*2] =
        { 0.0, 1.0,
          1.0, 1.0,
          1.0, 0.0,
          1.0, 0.0,
          0.0, 0.0,
          0.0, 1.0 };
    GLuint elements[6] =
        { 0, 1, 2,
          3, 4, 5 };
    
    if( ! quad && program ) {
        glGenVertexArrays(1, &quad);
        glBindVertexArray(quad);

        // vertices
        glGenBuffers(1, &vertices_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // texcoords
        glGenBuffers(1, &texcoords_id);
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
        
        GLint texcoord_position = glGetAttribLocation(program, "texcoord");
        glEnableVertexAttribArray(texcoord_position);
        glVertexAttribPointer(texcoord_position, 2, GL_FLOAT, GL_FALSE, 0, 0);

        // elements
        glGenBuffers(1, &elements_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);       
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if( quad && program ) {
        glBindVertexArray(quad);

        glUseProgram(program);

        GLint color_loc = glGetUniformLocation(program, "color");
        glUniform4f(color_loc, 1.0, 0.0, 0.0, 1.0);

        GLint normal_loc = glGetUniformLocation(program, "normal");
        glUniform3f(color_loc, 0.0, 0.0, 1.0);

        GLint projection_loc = glGetUniformLocation(program, "projection_matrix");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);
        
        GLint view_loc = glGetUniformLocation(program, "view_matrix");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);

        GLint model_loc = glGetUniformLocation(program, "model_matrix");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix);

        GLint diffuse_loc = glGetUniformLocation(program, "diffuse");
        glUniform1i(diffuse_loc, 0);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(0);
        glBindVertexArray(0);
    }
}
