/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "draw.h"

void draw_grid( float width,
                float height,
                int steps,
                const Color color,
                const Mat projection_matrix,
                const Mat view_matrix,
                const Mat model_matrix)
{
    const char* vertex_source =
        GLSL( uniform mat4 mvp_matrix;
              in vec3 vertex;
              uniform vec4 color;
              out vec4 frag_color;
              void main() {
                  gl_Position = mvp_matrix * vec4(vertex,1.0);
                  frag_color = color;
              });

    const char* fragment_source =
        GLSL( in vec4 frag_color;
              void main() {
                  gl_FragColor = frag_color;
              });

    static GLuint program = 0;
    if( ! program ) {
        program = glsl_make_program(vertex_source, fragment_source);
    }

    int size = (steps+1)*2 + (steps+1)*2;

    GLfloat vertices[size * 3];
    GLuint elements[size];

    GLuint grid = 0;
    GLuint vbo_ids[2] = { 0 };

    if( program ) {
        // 1  5  9 10----11
        // |  |  |
        // |  |  | 6-----7
        // |  |  |
        // 0  4  8 2-----3
        for( int i = 0; i < (steps+1); i++ ) {

            float xf = -width/2.0f + (float)i * (width / (float)steps);
            float yf = -height/2.0f + (float)i * (height / (float)steps);

            // a step includes one horizontal and one vertical line
            // made up of 2 vertices each, which makes 4 vertices in total
            // with 3 components which results in the number 12 below
            vertices[i * 12 + 0]  = xf;
            vertices[i * 12 + 1]  = -height/2.0f;
            vertices[i * 12 + 2]  = 0.0;

            elements[i * 4 + 0]   = i * 4 + 0;

            vertices[i * 12 + 3]  = xf;
            vertices[i * 12 + 4]  = height/2.0f ;
            vertices[i * 12 + 5]  = 0.0f;

            elements[i * 4 + 1]   = i * 4 + 1;

            vertices[i * 12 + 6]  = -width/2.0f;
            vertices[i * 12 + 7]  = yf;
            vertices[i * 12 + 8]  = 0.0;

            elements[i * 4 + 2]   = i * 4 + 2;

            vertices[i * 12 + 9]  = width/2.0f;
            vertices[i * 12 + 10] = yf;
            vertices[i * 12 + 11] = 0.0f;

            elements[i * 4 + 3]   = i * 4 + 3;
        }

        glGenVertexArrays(1, &grid);
        glBindVertexArray(grid);

        glGenBuffers(1, &vbo_ids[0]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[0]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

        glGenBuffers(1, &vbo_ids[1]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if( grid && program ) {
        glBindVertexArray(grid);

        glUseProgram(program);

        static GLint color_loc = -1;
        if( color_loc < 0 ) {
            color_loc = glGetUniformLocation(program, "color");
        }
        glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

        Mat mvp_matrix;
        mat_mul(model_matrix, view_matrix, mvp_matrix);
        mat_mul(mvp_matrix, projection_matrix, mvp_matrix);

        GLint mvp_loc = -1;
        if( mvp_loc < 0 ) {
            mvp_loc = glGetUniformLocation(program, "mvp_matrix");
        }
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp_matrix);

        glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glDeleteBuffers(2, vbo_ids);
        glDeleteVertexArrays(1, &grid);
    }
}

void draw_grid2( float width,
                float height,
                int steps,
                const Color color,
                const Mat projection_matrix,
                const Mat view_matrix,
                const Mat model_matrix)
{
    const char* vertex_source =
        GLSL( uniform mat4 mvp_matrix;
              in vec3 vertex;
              uniform vec4 color;
              out vec4 frag_color;
              uniform vec3 translation;
              uniform float width;
              uniform float height;
              void main() {
                  gl_Position = mvp_matrix * vec4(vertex[0] * width + translation[0],
                                                  vertex[1] * height + translation[1],
                                                  vertex[2] + translation[2],
                                                  1.0);
                  frag_color = color;
              });

    const char* fragment_source =
        GLSL( in vec4 frag_color;
              void main() {
                  gl_FragColor = frag_color;
              });

    static GLuint program = 0;
    if( ! program ) {
        program = glsl_make_program(vertex_source, fragment_source);
    }

    static GLfloat line_vertices[12] =
        { 0.5, -0.5, 0.0,
          -0.5, -0.5, 0.0,
          -0.5, 0.5, 0.0,
          -0.5, -0.5, 0.0 };
    static GLuint line_elements[4] =
        { 0, 1,
          2, 3 };

    static GLuint grid = 0;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;

    if( ! grid ) {
        glGenVertexArrays(1, &grid);
        glBindVertexArray(grid);

        glGenBuffers(1, &vertices_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id);

        glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices, GL_STATIC_DRAW);

        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

        glGenBuffers(1, &elements_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(line_elements), line_elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if( grid && program ) {
        glBindVertexArray(grid);

        glUseProgram(program);

        static GLint color_loc = -1;
        if( color_loc < 0 ) {
            color_loc = glGetUniformLocation(program, "color");
        }
        glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

        Mat mvp_matrix;
        mat_mul(model_matrix, view_matrix, mvp_matrix);
        mat_mul(mvp_matrix, projection_matrix, mvp_matrix);

        static GLint mvp_loc = -1;
        if( mvp_loc < 0 ) {
            mvp_loc = glGetUniformLocation(program, "mvp_matrix");
        }
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp_matrix);

        static GLint width_loc = -1;
        if( width_loc < 0 ) {
            width_loc = glGetUniformLocation(program, "width");
        }
        glUniform1f(width_loc, width);

        static GLint height_loc = -1;
        if( height_loc < 0 ) {
            height_loc = glGetUniformLocation(program, "height");
        }
        glUniform1f(height_loc, height);

        static GLint translation_loc = -1;
        if( translation_loc < 0 ) {
            translation_loc = glGetUniformLocation(program, "translation");
        }

        for( int i = 0; i <= steps; i++) {
            glUniform3f(translation_loc, 0.0, i*(height/steps), 0.0);
            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

            glUniform3f(translation_loc, i*(width/steps), 0.0, 0.0);
            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (const void*)(2 * sizeof(unsigned int)));

        }

        glBindVertexArray(0);
    }
}

void draw_grid3( float width,
                 float height,
                 int steps,
                 const Color color,
                 const Mat projection_matrix,
                 const Mat view_matrix,
                 const Mat model_matrix)
{
    const char* vertex_source =
        GLSL( uniform mat4 mvp_matrix;
              in vec3 vertex;
              uniform vec4 color;
              out vec4 frag_color;
              uniform float width;
              uniform float height;
              void main() {
                  gl_Position = mvp_matrix * vec4(vertex[0] * width,
                                                  vertex[1] * height,
                                                  0.0,
                                                  1.0);


                  frag_color = color;
              });

    const char* fragment_source =
        GLSL( in vec4 frag_color;
              void main() {
                  gl_FragColor = frag_color;
              });

    static GLuint program = 0;
    if( ! program ) {
        program = glsl_make_program(vertex_source, fragment_source);
    }

    static GLuint grid[8193] = { 0 };
    static GLuint vertices_id[8193] = { 0 };
    static GLuint elements_id[8193] = { 0 };

    if( ! grid[0] ) {
        for( int i = 1; i < 8193; i++ ) {
            grid[i] = 0;
            vertices_id[i] = 0;
            elements_id[i] = 0;
        }
        grid[0] = 1;
    }

    if( ! grid[steps] ) {
        // 1  5  9 10----11
        // |  |  |
        // |  |  | 6-----7
        // |  |  |
        // 0  4  8 2-----3
        //GLfloat vertices[4 * (10+1) * 3];
        //GLuint elements[4 * (10+1)];

        size_t vertices_size = 4 * (steps+1) * 3 * sizeof(float);
        size_t elements_size = 4 * (steps+1) * sizeof(unsigned int);
        GLfloat* vertices = malloc(vertices_size);
        GLuint* elements = malloc(elements_size);

        float xf = -0.5;
        float yf = -0.5;
        for( int i = 0; i <= steps; i++ ) {
            vertices[i * 12 + 0] = xf;
            vertices[i * 12 + 1] = -0.5;
            vertices[i * 12 + 2] = 1.0;

            elements[i * 4 + 0] = i * 4 + 0;

            vertices[i * 12 + 3] = xf;
            vertices[i * 12 + 4] = 0.5;
            vertices[i * 12 + 5] = 1.0f;

            elements[i * 4 + 1] = i * 4 + 1;

            vertices[i * 12 + 6] = -0.5;
            vertices[i * 12 + 7] = yf;
            vertices[i * 12 + 8] = -1.0;

            elements[i * 4 + 2] = i * 4 + 2;

            vertices[i * 12 + 9] = 0.5;
            vertices[i * 12 + 10] = yf;
            vertices[i * 12 + 11] = -1.0f;

            elements[i * 4 + 3] = i * 4 + 3;

            xf += 1.0/steps;
            yf += 1.0/steps;
        }

        glGenVertexArrays(1, &grid[steps]);
        glBindVertexArray(grid[steps]);

        glGenBuffers(1, &vertices_id[steps]);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id[steps]);

        glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

        glGenBuffers(1, &elements_id[steps]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id[steps]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements_size, elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        free(vertices);
        free(elements);
    }

    if( steps > 0 && grid[steps] && program ) {
        int size = (steps+1)*2 + (steps+1)*2;

        glBindVertexArray(grid[steps]);

        glUseProgram(program);

        static GLint color_loc = -1;
        if( color_loc < 0 ) {
            color_loc = glGetUniformLocation(program, "color");
        }
        glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

        Mat mvp_matrix;
        mat_mul(model_matrix, view_matrix, mvp_matrix);
        mat_mul(mvp_matrix, projection_matrix, mvp_matrix);

        static GLint mvp_loc = -1;
        if( mvp_loc < 0 ) {
            mvp_loc = glGetUniformLocation(program, "mvp_matrix");
        }
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp_matrix);

        static GLint width_loc = -1;
        if( width_loc < 0 ) {
            width_loc = glGetUniformLocation(program, "width");
        }
        glUniform1f(width_loc, width);

        static GLint height_loc = -1;
        if( height_loc < 0 ) {
            height_loc = glGetUniformLocation(program, "height");
        }
        glUniform1f(height_loc, height);

        glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}

void draw_grid4( float width,
                 float height,
                 int steps,
                 const Color color,
                 const Mat projection_matrix,
                 const Mat view_matrix,
                 const Mat model_matrix)
{
    const char* vertex_source =
        GLSL( uniform mat4 mvp_matrix;
              in vec3 vertex;
              uniform vec4 color;
              out vec4 frag_color;
              uniform float width;
              uniform float height;
              in vec3 barycentric;
              out vec3 vbc;
              void main() {
                  gl_Position = mvp_matrix * vec4(vertex[0] * width,
                                                  vertex[1] * height,
                                                  vertex[2],
                                                  1.0);
                  frag_color = vec4(barycentric, 1.0);
                  vbc = barycentric;
              });

    const char* fragment_source =
        GLSL( in vec4 frag_color;
              in vec3 vbc;

              float edge_factor(){
                  vec3 d = fwidth(vbc);
                  vec3 a3 = smoothstep(vec3(0.0), d*1.5, vbc);
                  //return min(min(a3.x, a3.y), a3.z);
                  return min(a3.x, a3.z);
              }

              void main() {
                  //gl_FragColor.rgb = mix(vec3(0.0), vec3(0.5), edge_factor());
                  gl_FragColor = vec4(1.0, 0.0, 0.0, (1.0 - edge_factor()) * 0.95);

                  /* if( vbc[0] < 0.02 || vbc[2] < 0.02 ) { */
                  /*     gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); */
                  /* } else { */
                  /*     gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0); */
                  /* } */
              });

    static GLuint program = 0;
    if( ! program ) {
        program = glsl_make_program(vertex_source, fragment_source);
    }

    static GLuint grid[8193] = { 0 };
    static GLuint vertices_id[8193] = { 0 };
    static GLuint elements_id[8193] = { 0 };
    static GLuint barycentrics_id[8193] = { 0 };

    if( ! grid[0] ) {
        for( int i = 1; i < 8193; i++ ) {
            grid[i] = 0;
            vertices_id[i] = 0;
            elements_id[i] = 0;
            barycentrics_id[i] = 0;
        }
        grid[0] = 1;
    }

    if( ! grid[steps] ) {
        // 1  5  9 10----11
        // |  |  |
        // |  |  | 6-----7
        // |  |  |
        // 0  4  8 2-----3
        //GLfloat vertices[4 * (10+1) * 3];
        //GLuint elements[4 * (10+1)];

        // 2 rectangles, made of 4 vertices each, a 3 components
        size_t vertices_size = 2 * 4 * 3 * steps * sizeof(float);
        // 2 rectangles, made of 2 triangles, with 3 indices each
        size_t elements_size = 2 * 2 * 3 * steps * sizeof(unsigned int);
        size_t barycentrics_size = vertices_size;

        GLfloat* vertices = malloc(vertices_size);
        GLuint* elements = malloc(elements_size);
        GLfloat* barycentrics = malloc(barycentrics_size);

        float xf = -0.5;
        float yf = -0.5;
        float step_amount = 1.0/steps;
        for( int i = 0; i < steps; i++ ) {
            unsigned int j = i * 2 * 4;
            unsigned int a = j + 0;
            unsigned int b = j + 1;
            unsigned int c = j + 2;
            unsigned int d = j + 3;

            vertices[a * 3 + 0] = xf;
            vertices[a * 3 + 1] = -0.5;
            vertices[a * 3 + 2] = 0.0;

            barycentrics[a * 3 + 0] = 1.0;
            barycentrics[a * 3 + 1] = 0.0;
            barycentrics[a * 3 + 2] = 0.0;

            vertices[b * 3 + 0] = xf;
            vertices[b * 3 + 1] = 0.5;
            vertices[b * 3 + 2] = 0.0;

            barycentrics[b * 3 + 0] = 0.0;
            barycentrics[b * 3 + 1] = 1.0;
            barycentrics[b * 3 + 2] = 0.0;

            vertices[c * 3 + 0] = xf+step_amount;
            vertices[c * 3 + 1] = 0.5;
            vertices[c * 3 + 2] = 0.0;

            barycentrics[c * 3 + 0] = 0.0;
            barycentrics[c * 3 + 1] = 0.0;
            barycentrics[c * 3 + 2] = 1.0;

            vertices[d * 3 + 0] = xf+step_amount;
            vertices[d * 3 + 1] = -0.5;
            vertices[d * 3 + 2] = 0.0;

            barycentrics[d * 3 + 0] = 0.0;
            barycentrics[d * 3 + 1] = 1.0;
            barycentrics[d * 3 + 2] = 0.0;

            unsigned int e = j + 4;
            unsigned int f = j + 5;
            unsigned int g = j + 6;
            unsigned int h = j + 7;

            vertices[e * 3 + 0] = -0.5;
            vertices[e * 3 + 1] = yf+step_amount;
            vertices[e * 3 + 2] = 0.0;

            barycentrics[e * 3 + 0] = 1.0;
            barycentrics[e * 3 + 1] = 0.0;
            barycentrics[e * 3 + 2] = 0.0;

            vertices[f * 3 + 0] = 0.5;
            vertices[f * 3 + 1] = yf+step_amount;
            vertices[f * 3 + 2] = 0.0;

            barycentrics[f * 3 + 0] = 0.0;
            barycentrics[f * 3 + 1] = 1.0;
            barycentrics[f * 3 + 2] = 0.0;

            vertices[g * 3 + 0] = 0.5;
            vertices[g * 3 + 1] = yf;
            vertices[g * 3 + 2] = 0.0;

            barycentrics[g * 3 + 0] = 0.0;
            barycentrics[g * 3 + 1] = 0.0;
            barycentrics[g * 3 + 2] = 1.0;

            vertices[h * 3 + 0] = -0.5;
            vertices[h * 3 + 1] = yf;
            vertices[h * 3 + 2] = 0.0;

            barycentrics[h * 3 + 0] = 0.0;
            barycentrics[h * 3 + 1] = 1.0;
            barycentrics[h * 3 + 2] = 0.0;

            unsigned int k = i * 2 * 2 * 3;
            elements[k + 0] = a;
            elements[k + 1] = b;
            elements[k + 2] = c;

            elements[k + 3] = c;
            elements[k + 4] = d;
            elements[k + 5] = a;

            elements[k + 6] = e;
            elements[k + 7] = f;
            elements[k + 8] = g;

            elements[k + 9] = g;
            elements[k + 10] = h;
            elements[k + 11] = e;

            xf += step_amount;
            yf += step_amount;
        }

        glGenVertexArrays(1, &grid[steps]);
        glBindVertexArray(grid[steps]);

        glGenBuffers(1, &vertices_id[steps]);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id[steps]);
        glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

        glGenBuffers(1, &barycentrics_id[steps]);
        glBindBuffer(GL_ARRAY_BUFFER, barycentrics_id[steps]);
        glBufferData(GL_ARRAY_BUFFER, barycentrics_size, barycentrics, GL_STATIC_DRAW);

        GLint barycentric_position = glGetAttribLocation(program, "barycentric");
        glEnableVertexAttribArray(barycentric_position);
        glVertexAttribPointer(barycentric_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

        glGenBuffers(1, &elements_id[steps]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id[steps]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements_size, elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        free(vertices);
        free(elements);
        free(barycentrics);
    }

    if( steps > 0 && grid[steps] && program ) {

        glBindVertexArray(grid[steps]);

        glUseProgram(program);

        static GLint color_loc = -1;
        if( color_loc < 0 ) {
            color_loc = glGetUniformLocation(program, "color");
        }
        glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

        Mat mvp_matrix;
        mat_mul(model_matrix, view_matrix, mvp_matrix);
        mat_mul(mvp_matrix, projection_matrix, mvp_matrix);

        static GLint mvp_loc = -1;
        if( mvp_loc < 0 ) {
            mvp_loc = glGetUniformLocation(program, "mvp_matrix");
        }
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp_matrix);

        static GLint width_loc = -1;
        if( width_loc < 0 ) {
            width_loc = glGetUniformLocation(program, "width");
        }
        glUniform1f(width_loc, width);

        static GLint height_loc = -1;
        if( height_loc < 0 ) {
            height_loc = glGetUniformLocation(program, "height");
        }
        glUniform1f(height_loc, height);

        size_t count = 2 * 2 * 3 * steps;
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}

void draw_arrow( const Vec v,
                 const Vec pos,
                 float scale,
                 float offset,
                 const Color color,
                 const Mat projection_matrix,
                 const Mat view_matrix,
                 const Mat model_matrix)
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
        program = glsl_make_program(vertex_source, fragment_source);
    }

    static GLuint arrow = 0;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;
    static GLfloat arrow_vertices[15] =
        { 0.0,  0.0,  0.0,
          0.1,  0.0,  -0.2,
          0.0,  0.1,  -0.2,
          -0.1, 0.0,  -0.2,
          0.0,  -0.1, -0.2 };

    static GLuint arrow_elements[16] =
        { 0, 1,
          0, 2,
          0, 3,
          0, 4,
          1, 2,
          2, 3,
          3, 4,
          4, 1 };

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

        static GLint color_loc = -1;
        if( color_loc < 0 ) {
            color_loc = glGetUniformLocation(program, "color");
        }
        glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

        static GLint projection_loc = -1;
        if( projection_loc < 0 ) {
            projection_loc = glGetUniformLocation(program, "projection_matrix");
        }
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);

        static GLint view_loc = -1;
        if( view_loc < 0 ) {
            view_loc = glGetUniformLocation(program, "view_matrix");
        }
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);

        Mat arrow_matrix;
        mat_identity(arrow_matrix);

        Vec z = { 0.0, 0.0, 1.0, 1.0 };
        Vec axis;
        vec_cross(v,z,axis);
        if( vnullp(axis) ) {
            vec_perpendicular(z,axis);
        }

        float angle;
        vec_angle(v,z,&angle);

        Quat rotation;
        quat_rotating(axis, angle, rotation);
        quat_mat(rotation, arrow_matrix);

        mat_scale(arrow_matrix, (Vec){scale, scale, scale}, arrow_matrix);

        Vec translation;
        vec_mul1f(v, scale, translation);
        vec_mul1f(translation, offset, translation);
        vec_add(pos, translation, translation);

        mat_translate(arrow_matrix, translation, arrow_matrix);
        mat_mul(arrow_matrix, model_matrix, arrow_matrix);

        static GLint model_loc = -1;
        if( model_loc < 0 ) {
            model_loc = glGetUniformLocation(program, "model_matrix");
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, arrow_matrix);

        glDrawElements(GL_LINES, 16, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}

void draw_vec( const Vec v,
               const Vec pos,
               float scale,
               const Color color,
               const Mat projection_matrix,
               const Mat view_matrix,
               const Mat model_matrix)
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
        program = glsl_make_program(vertex_source, fragment_source);
    }

    static GLuint vec = 0;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;
    /* static GLfloat vec_vertices[39] = */
    /*     { 0.01, 0.0, 0.0, */
    /*       0.01, 0.0, 0.8, */
    /*       -0.01, 0.0, 0.8, */
    /*       -0.01, 0.0, 0.0, */
    /*       0.0, 0.01, 0.0, */
    /*       0.0, 0.01, 0.8, */
    /*       0.0, -0.01, 0.8, */
    /*       0.0, -0.01, 0.0, */
    /*       0.0, 0.0, 1.0, */
    /*       0.1, 0.0, 0.8, */
    /*       0.0, 0.1, 0.8, */
    /*       -0.1, 0.0, 0.8, */
    /*       0.0, -0.1, 0.8 }; */
    static GLfloat vec_vertices[18] =
        { 0.0,  0.0,  0.0,
          0.0,  0.0,  0.5,
          0.05,  0.0,  0.4,
          0.0,  0.05,  0.4,
          -0.05, 0.0,  0.4,
          0.0,  -0.05, 0.4 };
    /* static GLuint vec_elements[24] = */
    /*     { 0, 1, 2, */
    /*       2, 3, 0, */
    /*       4, 5, 6, */
    /*       6, 7, 4, */
    /*       8, 9, 10, */
    /*       8, 10, 11, */
    /*       8, 11, 12, */
    /*       8, 12, 9 }; */
    static GLuint vec_elements[18] =
        { 0, 1,
          1, 2,
          1, 3,
          1, 4,
          1, 5,
          2, 3,
          3, 4,
          4, 5,
          5, 2 };

    if( ! vec ) {
        glGenVertexArrays(1, &vec);
        glBindVertexArray(vec);

        glGenBuffers(1, &vertices_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec_vertices), vec_vertices, GL_STATIC_DRAW);

        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

        glGenBuffers(1, &elements_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vec_elements), vec_elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if( vec && program ) {
        glBindVertexArray(vec);

        glUseProgram(program);

        static GLint color_loc = -1;
        if( color_loc < 0 ) {
            color_loc = glGetUniformLocation(program, "color");
        }
        glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

        static GLint projection_loc = -1;
        if( projection_loc < 0 ) {
            projection_loc = glGetUniformLocation(program, "projection_matrix");
        }
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);

        static GLint view_loc = -1;
        if( view_loc < 0 ) {
            view_loc = glGetUniformLocation(program, "view_matrix");
        }
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);

        Mat arrow_matrix;
        mat_identity(arrow_matrix);

        Vec z = { 0.0, 0.0, 1.0, 1.0 };
        Vec axis;
        vec_cross(v,z,axis);
        if( vnullp(axis) ) {
            vec_perpendicular(z,axis);
        }

        float angle;
        vec_angle(v,z,&angle);

        Quat rotation;
        quat_rotating(axis, angle, rotation);
        quat_mat(rotation, arrow_matrix);

        float length = 1.0;
        vec_length(v, &length);
        mat_scale(arrow_matrix, (Vec){scale * length, scale * length, scale * length}, arrow_matrix);

        mat_translate(arrow_matrix, pos, arrow_matrix);
        mat_mul(arrow_matrix, model_matrix, arrow_matrix);

        static GLint model_loc = -1;
        if( model_loc < 0 ) {
            model_loc = glGetUniformLocation(program, "model_matrix");
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, arrow_matrix);

        glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}

void draw_circle( const Vec normal,
                  float radius,
                  float start,
                  float end,
                  float arrow,
                  const Color color,
                  const Mat projection_matrix,
                  const Mat view_matrix,
                  const Mat model_matrix)
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
        program = glsl_make_program(vertex_source, fragment_source);
    }

    static GLuint circle = 0;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;
    static GLfloat circle_vertices[360*3];
    GLint circle_elements[360*2];

    int start_index = (start * 360.0)/(2.0 * PI);
    int end_index = (end * 360.0)/(2.0 * PI);
    for( int i = 0; i < end_index - start_index; i++ ) {
        circle_elements[i*2+0] = start_index + i;
        circle_elements[i*2+1] = start_index + i + 1;
    }
    if( start_index == 0 && end_index == 360 ) {
        circle_elements[360*2-1] = 0;
    }

    if( ! circle ) {
        for( int i = 0; i < 360; i++ ) {
            float theta = 2.0 * PI * (float)i / (float)360;
            float x = cosf(theta);
            float y = sinf(theta);

            circle_vertices[i*3+0] = x;
            circle_vertices[i*3+1] = y;
            circle_vertices[i*3+2] = 0.0;
        }

        glGenVertexArrays(1, &circle);
        glBindVertexArray(circle);

        glGenBuffers(1, &vertices_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circle_vertices), circle_vertices, GL_STATIC_DRAW);

        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

        glGenBuffers(1, &elements_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circle_elements), circle_elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if( circle && program ) {
        glBindVertexArray(circle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circle_elements), circle_elements, GL_STATIC_DRAW);

        glUseProgram(program);

        static GLint color_loc = -1;
        if( color_loc < 0 ) {
            color_loc = glGetUniformLocation(program, "color");
        }
        glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

        static GLint projection_loc = -1;
        if( projection_loc < 0 ) {
            projection_loc = glGetUniformLocation(program, "projection_matrix");
        }
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);

        static GLint view_loc = -1;
        if( view_loc < 0 ) {
            view_loc = glGetUniformLocation(program, "view_matrix");
        }
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);

        Mat circle_matrix = IDENTITY_MAT;

        float angle = 0.0;
        vec_angle((Vec){0.0, 0.0, 1.0, 1.0}, normal, &angle);

        if( angle > 0.0 ) {
            Vec axis = NULL_VEC;
            vec_cross((Vec){0.0, 0.0, 1.0, 1.0}, normal, axis);

            mat_rotating(axis, angle, circle_matrix);
        }

        mat_scale(circle_matrix, (Vec){radius, radius, radius}, circle_matrix);
        mat_mul(circle_matrix, model_matrix, circle_matrix);

        static GLint model_loc = -1;
        if( model_loc < 0 ) {
            model_loc = glGetUniformLocation(program, "model_matrix");
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, circle_matrix);

        glDrawElements(GL_LINES, (end_index - start_index)*2, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }

    if( arrow > 0.0 ) {
        int arrow_index = (start * 360.0)/(2.0 * PI);

        GLint i = circle_elements[arrow_index*2+0];
        GLint j = circle_elements[arrow_index*2+1];

        Vec a, b, v;
        a[0] = circle_vertices[i*3+0] * radius;
        a[1] = circle_vertices[i*3+1] * radius;
        a[2] = circle_vertices[i*3+2] * radius;
        a[3] = 1.0;

        b[0] = circle_vertices[j*3+0] * radius;
        b[1] = circle_vertices[j*3+1] * radius;
        b[2] = circle_vertices[j*3+2] * radius;
        b[3] = 1.0;

        vec_sub(a,b,v);
        draw_arrow(v, a, radius, 0.0, color, projection_matrix, view_matrix, model_matrix);
    }
}

void draw_pivot( float scale,
                 const Mat projection_matrix,
                 const Mat view_matrix,
                 const Mat model_matrix)
{
    draw_vec((Vec){2.0, 0.0, 0.0, 1.0}, (Vec){0.0, 0.0, 0.0, 1.0}, scale, (Color){1.0, 0.0, 0.0, 1.0}, projection_matrix, view_matrix, model_matrix);
    draw_vec((Vec){0.0, 2.0, 0.0, 1.0}, (Vec){0.0, 0.0, 0.0, 1.0}, scale, (Color){0.0, 1.0, 0.0, 1.0}, projection_matrix, view_matrix, model_matrix);
    draw_vec((Vec){0.0, 0.0, 2.0, 1.0}, (Vec){0.0, 0.0, 0.0, 1.0}, scale, (Color){0.0, 0.0, 1.0, 1.0}, projection_matrix, view_matrix, model_matrix);
}

void draw_physics( const Vec position,
                   float mass,
                   const Mat inertia,
                   const Vec linear_velocity,
                   const Vec angular_velocity,
                   float scale,
                   const Mat projection_matrix,
                   const Mat view_matrix)
{
    Mat model_matrix = IDENTITY_MAT;
    mat_translate(model_matrix, position, model_matrix);
    draw_vec(linear_velocity, (Vec){0.0, 0.0, 0.0, 1.0}, scale, (Color){1.0, 1.0, 0.0, 1.0}, projection_matrix, view_matrix, model_matrix);
    //draw_vec(angular_velocity, (Vec){0.0, 0.0, 0.0, 1.0}, scale, (Color){1.0, 1.0, 1.0, 1.0}, projection_matrix, view_matrix, model_matrix);
}

void draw_normals_array( const float* vertices,
                         const float* normals,
                         int n,
                         float scale,
                         const Color color,
                         const Mat projection_matrix,
                         const Mat view_matrix,
                         const Mat model_matrix )
{
    for( int i = 0; i < n; i++ ) {
        Mat arrow_matrix;
        mat_identity(arrow_matrix);

        Vec normal = { normals[i*3+0], normals[i*3+1], normals[i*3+2], 1.0f };
        Vec vertex = { vertices[i*3+0], vertices[i*3+1], vertices[i*3+2], 1.0 };

        draw_vec(normal, vertex, scale, color, projection_matrix, view_matrix, model_matrix);
    }

}

void draw_normals_buffer( GLuint vertices,
                          GLuint normals,
                          int n,
                          float scale,
                          const Color color,
                          const Mat projection_matrix,
                          const Mat view_matrix,
                          const Mat model_matrix)
{
}

void draw_texture_quad( GLuint texture_id,
                        const Mat projection_matrix,
                        const Mat view_matrix,
                        const Mat model_matrix )
{
    const char* vertex_source =
        GLSL( uniform mat4 projection_matrix;
              uniform mat4 model_matrix;
              uniform mat4 view_matrix;
              in vec3 vertex;
              in vec2 texcoord;
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
              in vec4 frag_color;
              in vec2 frag_texcoord;
              void main() {
                  gl_FragColor = texture(diffuse, frag_texcoord);
              });

    static GLuint program = 0;
    if( ! program ) {
        program = glsl_make_program(vertex_source, fragment_source);
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

        static GLint color_loc = -1;
        if( color_loc < 0 ) {
            color_loc = glGetUniformLocation(program, "color");
        }
        glUniform4f(color_loc, 1.0, 0.0, 0.0, 1.0);

        static GLint projection_loc = -1;
        if( projection_loc < 0 ) {
            projection_loc = glGetUniformLocation(program, "projection_matrix");
        }
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);

        static GLint view_loc = -1;
        if( view_loc < 0 ) {
            view_loc = glGetUniformLocation(program, "view_matrix");
        }
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);

        static GLint model_loc = -1;
        if( model_loc < 0 ) {
            model_loc = glGetUniformLocation(program, "model_matrix");
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix);

        GLint diffuse_loc = glGetUniformLocation(program, "diffuse");
        glUniform1i(diffuse_loc, 0);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}
