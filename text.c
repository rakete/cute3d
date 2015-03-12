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

#include "text.h"

void font_create(const wchar_t* unicode_alphabet, bool unicode, struct Character* symbols, struct Font* font) {
    font->unicode = unicode;
    font->kerning = 0.2;
    font->linespacing = 0.2;
    font->color[0] = 1.0;
    font->color[1] = 1.0;
    font->color[2] = 1.0;
    font->color[3] = 1.0;

    //int n = strlen(alphabet);

    int n = wcslen(unicode_alphabet);
    char ascii_alphabet[n + 1];
    size_t size = wcstombs(ascii_alphabet, unicode_alphabet, n);
    if( size >= n ) {
        ascii_alphabet[n] = '\0';
    }

    int max_h = 0;
    int widths[n];
    for( int i = 0; i < n; i++ ) {
        unsigned char c = ascii_alphabet[i];
        if( symbols[c].h > max_h ) {
            max_h = symbols[c].h;
        }
        widths[i] = symbols[c].w;
    }

    //    1     2     3     4     5     6   7   8   9  10  11  12  13
    //    A     B     C     D     E     F   G   H   I   J   K   L   M
    // w: 6     6     6     6     5     5   6   6   3   6   6   6   6
    // h: 7     7     7     7     7     7   7   7   7   7   7   7   7
    //
    // 7*13 7*6+7 7*4+7 3*7+7 2*7+7 2*7+7 7+7 7+7 7+7 7+7 7+7 7+7   7 column_height
    //    6    12    18    24    29     5  11  17  20  26  32   6  12 row_width
    //    8    16    32    32    32    32  32  32  32  32  32  32  32 power2
    //    12   18    24    29    34    11  17  20  26  32  38  12 nan row_width + widths[i+1]
    //    0     0     0     0     0     1   1   1   1   1   1   2   2 row_n
    int power2 = 8;
    int row_width = 0;
    int row_n = 0;
    int rows[n];
    int row_offsets[n];
    for( int i = 0; i < n; i++ ) {
        row_offsets[i] = row_width;

        int column_height = max_h * (n / (i+1)) + (n % (i+1) > 0) * max_h;

        row_width += widths[i];
        while( power2 < row_width ) {
            power2 *= 2;
        }

        int next_row_width = row_width + widths[i+1];
        /* printf("%d %d %d %d %d\n", column_height, row_width, power2, next_row_width, row_n); */
        rows[i] = row_n;
        if( (i+1) < n && column_height <= power2 && next_row_width > power2 ) {
            row_n += 1;
            row_width = 0;
        }
    }

    int32_t texture_size = power2 * power2;
    float* texture = (float*)calloc( texture_size * 4,  sizeof(float) );
    if( texture != NULL ) {
        /* for( int i = 0; i < texture_size; i++ ) { */
        /*     texture[i*4+0] = 0.0; */
        /*     texture[i*4+1] = 0.0; */
        /*     texture[i*4+2] = 0.0; */
        /*     texture[i*4+3] = 1.0; */
        /* } */

        unsigned char k = ascii_alphabet[0];
        struct Character c = symbols[k];
        for( int gy = 0; gy < c.h; gy++ ) {
            for( int gx = 0; gx < c.w; gx++ ) {
                texture[(gy*power2+gx)*4+0] = 1.0 * c.pixels[gy*c.w+gx];
                texture[(gy*power2+gx)*4+1] = 1.0 * c.pixels[gy*c.w+gx];
                texture[(gy*power2+gx)*4+2] = 1.0 * c.pixels[gy*c.w+gx];
                texture[(gy*power2+gx)*4+3] = 1.0 * c.pixels[gy*c.w+gx];
            }
        }

        for( int i = 0; i < n; i++ ) {
            unsigned char c = ascii_alphabet[i];
            font->alphabet[c] = 1;

            struct Glyph* glyph = &font->glyphs[c];

            int offset_x = row_offsets[i];
            int offset_y = rows[i] * max_h;
            int tx;
            int ty;

            glyph->x = offset_x;
            glyph->y = offset_y;
            glyph->w = symbols[c].w;
            glyph->h = max_h;

            for( int gy = 0; gy < symbols[c].h; gy++ ) {
                for( int gx = 0; gx < symbols[c].w; gx++ ) {
                    int pixel = symbols[c].pixels[gy * symbols[c].w + gx];
                    tx = offset_x + gx;
                    ty = offset_y + gy + max_h - symbols[c].h;

                    texture[(ty*power2+tx)*4+0] = 1.0 * pixel;
                    texture[(ty*power2+tx)*4+1] = 1.0 * pixel;
                    texture[(ty*power2+tx)*4+2] = 1.0 * pixel;
                    texture[(ty*power2+tx)*4+3] = 1.0 * pixel;
                }
            }
        }

        if( font->texture.id ) {
            glDeleteTextures(1,&font->texture.id);
        }

        glGenTextures(1, &font->texture.id);

        font->texture.width = power2;
        font->texture.height = power2;
        font->texture.type = GL_FLOAT;
        font->texture.format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, font->texture.id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        font->texture.min_filter = GL_NEAREST;
        font->texture.mag_filter = GL_NEAREST;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, power2, power2, 0, GL_RGBA, GL_FLOAT, texture);
        glBindTexture(GL_TEXTURE_2D, 0);

        free(texture);

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
                      vec2 dim = textureSize(diffuse,0);
                      float x = (1.0/dim.x) * (float(offset.x) + frag_texcoord.x * float(glyph.x));
                      float y = (1.0/dim.y) * (float(offset.y) + frag_texcoord.y * float(glyph.y));
                      gl_FragColor = texture(diffuse, vec2(x,y));
                      if( gl_FragColor.a < 0.05 ) {
                          discard;
                      }
                  });


        font->shader.program = glsl_make_program(vertex_source, fragment_source);
    }
}

void font_texture_filter(struct Font* font, GLint min_filter, GLint mag_filter) {
    glBindTexture(GL_TEXTURE_2D, font->texture.id);

    if( min_filter ) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        font->texture.min_filter = min_filter;
    }

    if( mag_filter ) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
        font->texture.mag_filter = mag_filter;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void text_put(const wchar_t* text, const struct Font* font, float scale, const Mat projection_matrix, const Mat view_matrix, Mat model_matrix) {
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
          -0.5, 0.5,  0.0 };
    GLfloat texcoords[6*2] =
        { 0.0, 0.0,
          1.0, 0.0,
          1.0, 1.0,
          1.0, 1.0,
          0.0, 1.0,
          0.0, 0.0 };
    GLuint elements[6] =
        { 0, 2, 1,
          3, 5, 4 };

    if( ! quad ) {
        glGenVertexArrays(1, &quad);
        glBindVertexArray(quad);

        // vertices
        glGenBuffers(1, &vertices_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        GLint vertex_position = glGetAttribLocation(font->shader.program, "vertex");
        glEnableVertexAttribArray(vertex_position);
        glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // texcoords
        glGenBuffers(1, &texcoords_id);
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

        GLint texcoord_position = glGetAttribLocation(font->shader.program, "texcoord");
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

    if( quad && font->texture.id ) {
        glBindVertexArray(quad);

        glUseProgram(font->shader.program);

        GLint color_loc = glGetUniformLocation(font->shader.program, "color");
        glUniform4f(color_loc, 1.0, 0.0, 0.0, 1.0);

        GLint normal_loc = glGetUniformLocation(font->shader.program, "normal");
        glUniform3f(normal_loc, 0.0, 0.0, 1.0);

        GLint projection_loc = glGetUniformLocation(font->shader.program, "projection_matrix");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);

        GLint view_loc = glGetUniformLocation(font->shader.program, "view_matrix");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);

        GLint diffuse_loc = glGetUniformLocation(font->shader.program, "diffuse");

        if( diffuse_loc > -1 ) {
            glUniform1i(diffuse_loc, 0);

            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, font->texture.id);

            GLint glyph_loc = glGetUniformLocation(font->shader.program, "glyph");
            GLint offset_loc = glGetUniformLocation(font->shader.program, "offset");
            GLint model_loc = glGetUniformLocation(font->shader.program, "model_matrix");

            if( glyph_loc > -1 ) {
                int length = wcslen(text);
                char ascii[length + 1];
                size_t textsize = wcstombs(ascii, text, length);
                if( textsize >= length ) {
                    ascii[length] = '\0';
                }

                Vec4f cursor_vec = {0.0,0.0,0.0,1.0};
                bool newline = 0;
                for( int i = 0; i < length; i++ ) {

                    const struct Glyph* glyph = NULL;
                    if( font->unicode ) {
                        wchar_t c = text[i];
                        if( c == '\n' ) {
                            newline = 1;
                            continue;
                        } else if( font->alphabet[c] ) {
                            glyph = &font->glyphs[c];
                        }
                    } else {
                        char c = ascii[i];
                        if( c == '\n' ) {
                            newline = 1;
                            continue;
                        } else if( font->alphabet[(int)c] ) {
                            glyph = &font->glyphs[(int)c];
                        }
                    }

                    if( newline && i > 0 ) {
                        float y = 1.0 + font->linespacing;
                        cursor_vec[0] = 0.0;
                        cursor_vec[1] -= y;
                    } else if( ! newline && i > 0 ) {
                        float x = 1.0 - font->kerning;
                        cursor_vec[0] += x;
                    }
                    newline = 0;

                    Mat glyph_matrix;
                    mat_identity(glyph_matrix);
                    mat_translate(glyph_matrix, cursor_vec, glyph_matrix);
                    mat_scale(glyph_matrix, (Vec){ scale, scale, scale, 1.0 }, glyph_matrix);
                    mat_mul(glyph_matrix, model_matrix, glyph_matrix);

                    if( glyph ) {
                        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glyph_matrix);
                        glUniform2i(glyph_loc, glyph->w, glyph->h);
                        glUniform2i(offset_loc, glyph->x, glyph->y);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    }
                }
            }
        }

        glBindVertexArray(0);
    }
}

void text_overlay(const wchar_t* text, const struct Font* font, int size, struct Camera camera, int x, int y) {
    camera.type = orthographic;
    Mat ortho_projection, ortho_view;
    camera_matrices(&camera, ortho_projection, ortho_view);

    Mat text_matrix;
    mat_identity(text_matrix);
    mat_rotate(text_matrix, camera.pivot.orientation, text_matrix);

    Vec translation;
    translation[0] = camera.frustum.left + fabs(camera.frustum.left - camera.frustum.right)/(float)camera.screen.width * (float)x;
    translation[1] = camera.frustum.top - fabs(camera.frustum.top - camera.frustum.bottom)/(float)camera.screen.height * (float)y;
    translation[2] = 0.0;
    translation[3] = 1.0;
    mat_translate(text_matrix, translation, text_matrix);

    float scale = (float)size/(float)camera.screen.height;

    glDisable(GL_DEPTH_TEST);
    text_put(text, font, scale, ortho_projection, ortho_view, text_matrix);
    glEnable(GL_DEPTH_TEST);
}
