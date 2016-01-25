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

#include "gui.h"

void text_put_old(const wchar_t* text, const struct Font* font, float scale, const Mat projection_matrix, const Mat view_matrix, Mat model_matrix) {
    static GLuint quad = 0;
    static GLuint vertices_id = 0;
    static GLuint texcoords_id = 0;
    static GLuint elements_id = 0;

    static GLfloat vertices[6*3] =
        { -0.5, 0.5, 0.0,
          0.5, 0.5, 0.0,
          0.5, -0.5, 0.0,
          0.5, -0.5, 0.0,
          -0.5, -0.5, 0.0,
          -0.5, 0.5,  0.0 };
    static GLfloat texcoords[6*2] =
        { 0.0, 0.0,
          1.0, 0.0,
          1.0, 1.0,
          1.0, 1.0,
          0.0, 1.0,
          0.0, 0.0 };
    static GLuint elements[6] =
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
        assert( vertex_position >= 0 );

        glEnableVertexAttribArray((GLuint)vertex_position);
        glVertexAttribPointer((GLuint)vertex_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // texcoords
        glGenBuffers(1, &texcoords_id);
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

        GLint texcoord_position = glGetAttribLocation(font->shader.program, "texcoord");
        assert( texcoord_position >= 0 );

        glEnableVertexAttribArray((GLuint)texcoord_position);
        glVertexAttribPointer((GLuint)texcoord_position, 2, GL_FLOAT, GL_FALSE, 0, 0);

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

        GLint diffuse_loc = glGetUniformLocation(font->shader.program, "diffuse_texture");

        if( diffuse_loc > -1 ) {
            glUniform1i(diffuse_loc, 0);

            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, font->texture.id);

            GLint glyph_loc = glGetUniformLocation(font->shader.program, "glyph");
            GLint offset_loc = glGetUniformLocation(font->shader.program, "offset");
            GLint model_loc = glGetUniformLocation(font->shader.program, "model_matrix");

            if( glyph_loc > -1 ) {
                uint32_t text_length = wcslen(text);
                char ascii[text_length + 1];
                size_t textsize = wcstombs(ascii, text, text_length);
                if( textsize >= text_length ) {
                    ascii[text_length] = '\0';
                }

                Vec4f cursor_translation = {0.0,0.0,0.0,1.0};
                bool newline = 0;
                for( uint32_t i = 0; i < text_length; i++ ) {
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
                        cursor_translation[0] = 0.0;
                        cursor_translation[1] -= y;
                    } else if( ! newline && i > 0 ) {
                        float x = 1.0 - font->kerning;
                        cursor_translation[0] += x;
                    }
                    newline = 0;

                    Mat glyph_matrix;
                    mat_identity(glyph_matrix);
                    mat_translate(glyph_matrix, cursor_translation, glyph_matrix);
                    mat_scale(glyph_matrix, scale, glyph_matrix);
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

void text_overlay(const wchar_t* text, const struct Font* font, int32_t size, struct Camera camera, int32_t x, int32_t y) {
    Mat ortho_projection, ortho_view;
    camera_matrices(&camera, CAMERA_ORTHOGRAPHIC, ortho_projection, ortho_view);

    Mat text_matrix;
    mat_identity(text_matrix);

    Vec translation;
    translation[0] = camera.frustum.left + fabs(camera.frustum.left - camera.frustum.right)/(float)camera.screen.width * (float)x;
    translation[1] = camera.frustum.top - fabs(camera.frustum.top - camera.frustum.bottom)/(float)camera.screen.height * (float)y;
    translation[2] = 0.0;
    translation[3] = 1.0;
    mat_translate(text_matrix, translation, text_matrix);

    mat_rotate(text_matrix, camera.pivot.orientation, text_matrix);

    // multiplying with zNear fixes text being too small or too big when
    // zNear is set to something other then 1.0
    float scale = (float)size / (float)camera.screen.height * camera.frustum.zNear;

    glDisable(GL_DEPTH_TEST);
    text_put(text, font, scale, ortho_projection, ortho_view, text_matrix);
    glEnable(GL_DEPTH_TEST);
}

static wchar_t show_buffer[SHOW_BUFSIZE];

void show_printf(const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);

    swprintf(show_buffer + wcslen(show_buffer), SHOW_BUFSIZE - wcslen(show_buffer), L"\n");
    vswprintf(show_buffer + wcslen(show_buffer), SHOW_BUFSIZE - wcslen(show_buffer), format, args);
    va_end(args);
}

void show_fps_counter(double delta) {
    static int32_t frames_done = 0;
    static double old_time = -1;
    static double game_time = 0;
    static double fps = 0;

    game_time += delta;

    if( old_time < 0 ) {
        old_time = game_time;
    }

    if( (game_time - old_time) >= 1.0 ) {
        fps = (double)frames_done / (game_time - old_time);
        frames_done = 0;
        old_time = game_time;
    }
    frames_done++;

    show_printf(L"FPS :%.1f", fps);
}

void show_time(double time) {
    int32_t hours = 0;
    int32_t minutes = 0;
    int32_t seconds = 0;
    int32_t milliseconds = 0;

    hours = (int)floor(time) / 60 / 60;
    minutes = (int)floor(time) / 60 % 60;
    seconds = (int)floor(time) % 60;
    milliseconds = (int)floor((time - floor(time)) * 1000);

    show_printf(L"TIME:%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
}

void show_vec(const char* title, Vec v) {
    show_printf(L"%s(%f %f %f %f)\n", title, v[0], v[1], v[2], v[3]);
}

void show_render(const struct Font* font, int32_t size, struct Camera camera) {
    if( font == NULL ) {
        static int32_t default_font_created = 0;
        static struct Font default_font;

        if( ! default_font_created ) {
            struct Character symbols[256];
            default_font_create(symbols);
            font_create(&default_font, L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;+-*/=()[]{}", false, symbols, "default");
            default_font_created = 1;
        }

        text_overlay(show_buffer, &default_font, size, camera, size, 0);
    } else {
        text_overlay(show_buffer, font, size, camera, size, 0);
    }

    show_buffer[0] = '\0';
}
