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
                Color color,
                Mat projection_matrix,
                Mat view_matrix,
                Mat model_matrix)
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

void draw_vec( Vec v,
               Vec pos,
               float scale,
               Color color,
               Mat projection_matrix,
               Mat view_matrix,
               Mat model_matrix)
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
    static GLfloat arrow_vertices[18] =
        { 0.0,  0.0,  0.0,
          0.0,  0.0,  0.5,
          0.05,  0.0,  0.4,
          0.0,  0.05,  0.4,
          -0.05, 0.0,  0.4,
          0.0,  -0.05, 0.4 };

    static GLuint arrow_elements[18] =
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
        quat_rotation(axis,angle,rotation);
        quat_mat(rotation, arrow_matrix);

        mat_scale(arrow_matrix, (Vec){scale, scale, scale}, arrow_matrix);

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

void draw_normals_array( float* vertices,
                         float* normals,
                         int n,
                         float scale,
                         Color color,
                         Mat projection_matrix,
                         Mat view_matrix,
                         Mat model_matrix )
{
    for( int i = 0; i < n; i++ ) {
        Mat arrow_matrix;
        mat_identity(arrow_matrix);

        Vec normal = { normals[i*3+0], normals[i*3+1], normals[i*3+2], 1.0f };
        Vec vertex = { vertices[i*3+0], vertices[i*3+1], vertices[i*3+2], 1.0 };

        draw_vec(normal, vertex, scale, color, projection_matrix, view_matrix, model_matrix);
    }

}

void draw_texture_quad( GLuint texture_id,
                        Mat projection_matrix,
                        Mat view_matrix,
                        Mat model_matrix )
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
