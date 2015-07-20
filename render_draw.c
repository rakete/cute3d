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

#include "render_draw.h"

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
        quat_from_axis_angle(axis, angle, rotation);
        quat_to_mat(rotation, arrow_matrix);

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

    static GLuint vec = 0;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;
    static GLfloat vec_vertices[18] =
        { 0.0,  0.0,  0.0,
          0.0,  0.0,  1.0,
          0.05,  0.0,  0.9,
          0.0,  0.05,  0.9,
          -0.05, 0.0,  0.9,
          0.0,  -0.05, 0.9 };
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
        quat_from_axis_angle(axis, angle, rotation);
        quat_to_mat(rotation, arrow_matrix);

        // mesh length is always 1.0f, so not only scale it with scale, but also scale it with the
        // actual length it should have
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

        if( arrow <= 0.0f ) {
            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
        } else if( arrow >= 1.0f ) {
            glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);
        } else {
            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

            Mat arrow_offset_matrix = IDENTITY_MAT;
            quat_to_mat(rotation, arrow_offset_matrix);
            mat_scale(arrow_offset_matrix, (Vec){scale * length, scale * length, scale * length}, arrow_offset_matrix);
            mat_translate(arrow_offset_matrix, pos, arrow_offset_matrix);

            // move the arrow along scaled vector down so that the value given in arrow will fit the
            // 1.0f == arrow at the top, 0.0f arrow invisible scheme
            // arrow == 0.9f -> arrow -10% (-1.0f + 0.9f) from the top of the vector
            mat_translate(arrow_offset_matrix, (Vec){0.0, (-1.0 + arrow) * scale * length, 0.0, 1.0}, arrow_offset_matrix);
            mat_mul(arrow_offset_matrix, model_matrix, arrow_offset_matrix);
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, arrow_offset_matrix);

            glDrawElements(GL_LINES, 16, GL_UNSIGNED_INT, (const void*)(2 * sizeof(unsigned int)));
        }

        glBindVertexArray(0);
    }
}

void draw_quat( const Quat q,
                float scale,
                const Color color1,
                const Color color2,
                const Mat projection_matrix,
                const Mat view_matrix,
                const Mat model_matrix )
{
    // visualizing quaternions is nasty, I am just drawing an axis-angle representation, that kind of sort of
    // works
    Vec axis;
    float angle;
    quat_axis_angle(q, axis, &angle);

    // a quaternion has a range of two rotations, so as if going from -2*PI to 2*PI, so when converting into
    // axis-angle representation, we get an axis and its inverse (like 1,0,0 and -1,0,0) first with angles
    // from 0 to 2*PI and then from 2*PI back to 0
    Vec axis_inverse;
    if( vsign(axis) < 0 ) {
        // so when the axis and the angle is fed to draw_circle, since it always changes direction, the circle arrow
        // will direction will change too, so this if makes sure that axis is always pointing in the same direction
        angle = 2*PI - angle;
        vec_mul1f(axis, -1.0f, axis);

        // we also draw two axis in different colors, that will swap, to indicate the two rotation phases of the quaternion
        // thats why this is in here and in the else with swapped colors, and thats why we flip the axis again (this time,
        // to get the actual inverse)
        draw_vec(axis, (Vec){0.0, 0.0, 0.0, 1.0}, scale, 1.0f, color1, projection_matrix, view_matrix, model_matrix);
        vec_mul1f(axis, -1.0f, axis_inverse);
        draw_vec(axis_inverse, (Vec){0.0, 0.0, 0.0, 1.0}, scale, 1.0f, color2, projection_matrix, view_matrix, model_matrix);
    } else {
        draw_vec(axis, (Vec){0.0, 0.0, 0.0, 1.0}, scale, 1.0f, color2, projection_matrix, view_matrix, model_matrix);
        vec_mul1f(axis, -1.0f, axis_inverse);
        draw_vec(axis_inverse, (Vec){0.0, 0.0, 0.0, 1.0}, scale, 1.0f, color1, projection_matrix, view_matrix, model_matrix);
    }

    // draw two circles from 0 to angle, and from angle to 2*PI, so we'll get a full circle consisting
    // of two parts in different colors
    Quat circle_rotation;
    quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, axis, circle_rotation);

    Mat circle_transform;
    mat_rotating(circle_rotation, circle_transform);
    mat_mul(circle_transform, model_matrix, circle_transform);

    draw_circle(scale, 0.0f, angle, 1.0f, color1, projection_matrix, view_matrix, circle_transform);
    draw_circle(scale, angle, 2*PI, 0.0f, color2, projection_matrix, view_matrix, circle_transform);
}

void draw_circle( float radius,
                  float start,
                  float end,
                  float arrow,
                  const Color color,
                  const Mat projection_matrix,
                  const Mat view_matrix,
                  const Mat model_matrix)
{
    if( end > 2*PI || end < 0.0f ) {
        end = 2*PI;
    }
    if( start < 0.0f || start > 2*PI ) {
        start = 0.0f;
    }

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
    for( int i = 0; i < 360*2; i++ ) {
        circle_elements[i] = 0;
    }

    int start_index = (start * 360.0)/(2.0 * PI);
    int end_index = (end * 360.0)/(2.0 * PI);
    for( int i = 0; i < end_index - start_index; i++ ) {
        circle_elements[i*2+0] = start_index + i;
        circle_elements[i*2+1] = start_index + i + 1;
    }

    if( ! circle ) {
        for( int i = 0; i < 360; i++ ) {
            float theta = -2.0 * PI * (float)i / (float)360;
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

        // the matrix for the circle needs to be scaled by radius, but drawing the arrow head will apply
        // its own scalation, so we need two seperate transformations, one for the circle, and one with
        // only the neccessary rotation and translation and without scalation for the arrow
        Mat circle_matrix = IDENTITY_MAT;
        Mat arrow_matrix = IDENTITY_MAT;
        mat_scale(arrow_matrix, (Vec){radius, radius, radius}, circle_matrix);
        mat_mul(circle_matrix, model_matrix, circle_matrix);
        mat_mul(arrow_matrix, model_matrix, arrow_matrix);

        static GLint model_loc = -1;
        if( model_loc < 0 ) {
            model_loc = glGetUniformLocation(program, "model_matrix");
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, circle_matrix);

        glDrawElements(GL_LINES, (end_index - start_index)*2-1, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        if( arrow > 0.0 && end_index > 1 ) {
            int arrow_index = end_index - start_index;

            GLint i = circle_elements[arrow_index*2-1];
            GLint j = circle_elements[arrow_index*2-2];

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
            draw_arrow(v, a, radius/2.0, 0.0, color, projection_matrix, view_matrix, arrow_matrix);
        }
    }
}

void draw_pivot( float scale,
                 const Mat projection_matrix,
                 const Mat view_matrix,
                 const Mat model_matrix)
{
    draw_vec((Vec){2.0, 0.0, 0.0, 1.0}, (Vec){0.0, 0.0, 0.0, 1.0}, scale, 1.0f, (Color){1.0, 0.0, 0.0, 1.0}, projection_matrix, view_matrix, model_matrix);
    draw_vec((Vec){0.0, 2.0, 0.0, 1.0}, (Vec){0.0, 0.0, 0.0, 1.0}, scale, 1.0f, (Color){0.0, 1.0, 0.0, 1.0}, projection_matrix, view_matrix, model_matrix);
    draw_vec((Vec){0.0, 0.0, 2.0, 1.0}, (Vec){0.0, 0.0, 0.0, 1.0}, scale, 1.0f, (Color){0.0, 0.0, 1.0, 1.0}, projection_matrix, view_matrix, model_matrix);
}

void draw_reticle(float scale,
                  const Color color,
                  const Mat projection_matrix,
                  const Mat view_matrix,
                  const Mat model_matrix)
{
    // bitmap: reticle
    // using: bnw
    /* static int reticle[16*16] = { */
    /*     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, */
    /*     0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, */
    /*     0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, */
    /*     0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 */
    /* }; */

    const char* vertex_source =
        GLSL( uniform mat4 projection_matrix;
              uniform mat4 model_matrix;
              uniform mat4 view_matrix;
              in vec3 vertex;
              uniform vec4 color;
              out vec4 frag_color;
              uniform float scale;
              void main() {
                  gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex, 1.0);
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

    static GLuint reticle = 0;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;
    GLfloat vertices[8*3] =
        { 0.0, 0.5, 0.0,
          0.0, 0.1, 0.0,
          0.0, -0.5, 0.0,
          0.0, -0.1, 0.0,
          0.5, 0.0, 0.0,
          0.1, 0.0, 0.0,
          -0.5, 0.0, 0.0,
          -0.1, 0.0, 0.0 };
    GLuint elements[8] =
        { 0, 1,
          2, 3,
          4, 5,
          6, 7 };

    if( ! reticle && program ) {
        glGenVertexArrays(1, &reticle);
        glBindVertexArray(reticle);

        // vertices
        glGenBuffers(1, &vertices_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        GLint vertex_position = glGetAttribLocation(program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // elements
        glGenBuffers(1, &elements_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if( reticle && program ) {
        glBindVertexArray(reticle);

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

        Mat reticle_matrix = IDENTITY_MAT;
        mat_scale(reticle_matrix, (Vec){scale, scale, scale}, reticle_matrix);
        mat_mul(reticle_matrix, model_matrix, reticle_matrix);

        static GLint model_loc = -1;
        if( model_loc < 0 ) {
            model_loc = glGetUniformLocation(program, "model_matrix");
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, reticle_matrix);

        glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}

void draw_contact( const Vec contact_point,
                   const Vec contact_normal,
                   float contact_penetration,
                   float scale,
                   const Mat projection_matrix,
                   const Mat view_matrix,
                   const Mat model_matrix)
{

    Mat contact_matrix;
    mat_translate(model_matrix, contact_point, contact_matrix);

    draw_vec(contact_normal, (Vec)NULL_VEC, scale, 1.0f, (Color){0.1, 0.9, 0.7, 1.0}, projection_matrix, view_matrix, contact_matrix);

    Quat q = NULL_VEC;
    quat_from_vec_pair((Vec)Z_AXIS, (Vec)Y_AXIS, q);
    quat_mul_axis_angle(q, (Vec)Y_AXIS, PI/4, q);

    Mat quad_matrix1;
    mat_rotating(q, quad_matrix1);
    mat_mul(quad_matrix1, contact_matrix, quad_matrix1);
    draw_color_quad(scale/2.0, (Color){0.1, 0.9, 0.7, 1.0}, projection_matrix, view_matrix, quad_matrix1);

    quat_mul_axis_angle(q, (Vec)X_AXIS, PI/2, q);

    Mat quad_matrix2;
    mat_rotating(q, quad_matrix2);
    mat_mul(quad_matrix2, contact_matrix, quad_matrix2);
    draw_color_quad(scale/2.0, (Color){0.1, 0.9, 0.7, 1.0}, projection_matrix, view_matrix, quad_matrix2);

    quat_mul_axis_angle(q, (Vec)Y_AXIS, PI/2, q);

    Mat quad_matrix3;
    mat_rotating(q, quad_matrix3);
    mat_mul(quad_matrix3, contact_matrix, quad_matrix3);
    draw_color_quad(scale/2.0, (Color){0.1, 0.9, 0.7, 1.0}, projection_matrix, view_matrix, quad_matrix3);

    Quat reticle_rotation;
    quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, (Vec){0.0, 1.0, 0.0, 1.0}, reticle_rotation);
    Mat reticle_transform;
    mat_rotating(reticle_rotation, reticle_transform);
    mat_mul(reticle_transform, contact_matrix, reticle_transform);
    draw_reticle(scale/2.0, (Color){1.0, 0.2, 0.7, 1.0}, projection_matrix, view_matrix, reticle_transform);

    Vec contact_normal_flipped;
    vec_mul1f(contact_normal, -1.0f, contact_normal_flipped);
    draw_vec(contact_normal_flipped, (Vec)NULL_VEC, scale, 0.0f, (Color){1.0, 0.2, 0.7, 1.0}, projection_matrix, view_matrix, contact_matrix);

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

        draw_vec(normal, vertex, scale, 0.0f, color, projection_matrix, view_matrix, model_matrix);
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

void draw_texture_quad( float scale,
                        GLuint texture_id,
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
              out vec4 frag_color;
              out vec2 frag_texcoord;
              void main() {
                  gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex,1.0);
                  frag_texcoord = texcoord;
              });

    const char* fragment_source =
        GLSL( uniform sampler2D diffuse;
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
        { -0.5, 0.5, 0.0,
          0.5, 0.5, 0.0,
          0.5, -0.5, 0.0,
          0.5, -0.5, 0.0,
          -0.5, -0.5, 0.0,
          -0.5, 0.5, 0.0 };
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

        Mat quad_matrix = IDENTITY_MAT;
        mat_scale(quad_matrix, (Vec){scale, scale, scale}, quad_matrix);
        mat_mul(quad_matrix, model_matrix, quad_matrix);

        static GLint model_loc = -1;
        if( model_loc < 0 ) {
            model_loc = glGetUniformLocation(program, "model_matrix");
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, quad_matrix);

        GLint diffuse_loc = glGetUniformLocation(program, "diffuse");
        glUniform1i(diffuse_loc, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}

void draw_color_quad( float scale,
                      const Color color,
                      const Mat projection_matrix,
                      const Mat view_matrix,
                      const Mat model_matrix )
{
    const char* vertex_source =
        GLSL( uniform mat4 projection_matrix;
              uniform mat4 model_matrix;
              uniform mat4 view_matrix;
              uniform vec4 color;
              in vec3 vertex;
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

    static GLuint quad = 0;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;
    GLfloat vertices[6*3] =
        { -0.5, 0.5, 0.0,
          0.5, 0.5, 0.0,
          0.5, -0.5, 0.0,
          0.5, -0.5, 0.0,
          -0.5, -0.5, 0.0,
          -0.5, 0.5, 0.0 };
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

        Mat quad_matrix = IDENTITY_MAT;
        mat_scale(quad_matrix, (Vec){scale, scale, scale}, quad_matrix);
        mat_mul(quad_matrix, model_matrix, quad_matrix);

        static GLint model_loc = -1;
        if( model_loc < 0 ) {
            model_loc = glGetUniformLocation(program, "model_matrix");
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, quad_matrix);

        GLint diffuse_loc = glGetUniformLocation(program, "diffuse");
        glUniform1i(diffuse_loc, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}

void draw_line_quad( float scale,
                     const Color color,
                     const Mat projection_matrix,
                     const Mat view_matrix,
                     const Mat model_matrix )
{
    const char* vertex_source =
        GLSL( uniform mat4 projection_matrix;
              uniform mat4 model_matrix;
              uniform mat4 view_matrix;
              in vec3 vertex;
              uniform vec4 color;
              out vec4 frag_color;
              uniform float scale;
              void main() {
                  gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex, 1.0);
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

    static GLuint quad = 0;
    static GLuint vertices_id = 0;
    static GLuint elements_id = 0;
    GLfloat vertices[4*3] =
        { -0.5, 0.5, 0.0,
          0.5, 0.5, 0.0,
          0.5, -0.5, 0.0,
          -0.5, -0.5, 0.0 };
    GLuint elements[8] =
        { 0, 1,
          1, 2,
          2, 3,
          3, 0 };

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

        Mat quad_matrix = IDENTITY_MAT;
        mat_scale(quad_matrix, (Vec){scale, scale, scale}, quad_matrix);
        mat_mul(quad_matrix, model_matrix, quad_matrix);

        static GLint model_loc = -1;
        if( model_loc < 0 ) {
            model_loc = glGetUniformLocation(program, "model_matrix");
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, quad_matrix);

        glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}
