/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#include "gui_text.h"

void text_put(struct Canvas* canvas,
              int32_t layer,
              Vec4f cursor,
              int32_t projection,
              const Mat model_matrix,
              const Color color,
              float scale,
              const char* font_name,
              const wchar_t* unicode_text)
{
    log_assert( layer >= 0 );
    log_assert( projection >= 0 );
    log_assert( canvas != NULL );
    log_assert( scale > 0.0f );
    log_assert( color != NULL );
    log_assert( model_matrix != NULL );

    size_t text_length = wcslen(unicode_text);
    if( text_length == 0 ) {
        log_warn(__FILE__, __LINE__, "no text to display\n");
        return;
    }

    Vec4f cursor_translation = {0.0, 0.0, 0.0, 1.0};
    if( cursor != NULL ) {
        vec_copy4f(cursor, cursor_translation);
    }

    log_assert( text_length < MAX_TEXT_PUT_SIZE );

    // - these used to be vla, but this seems to be an especially bad place to
    // have a possible stack overflow, so I wanted to replace these with mallocs,
    // but as an experiment I am going to try and use static arrays instead, these
    // should life in the .DATA segment and a not reduce stack space, yet come without
    // malloc overhead and don't have to be freed
    static float vertices[MAX_TEXT_PUT_SIZE * 4 * 3] = {0};
    static uint8_t colors[MAX_TEXT_PUT_SIZE * 4 * 4] = {0};
    static float texcoords[MAX_TEXT_PUT_SIZE * 4 * 2] = {0};
    static uint32_t indices[MAX_TEXT_PUT_SIZE * 6] = {0};

    int32_t font_i = canvas_find_font(canvas, font_name);
    if( font_i == MAX_CANVAS_FONTS ) {
        log_warn(__FILE__, __LINE__, "font %s not found\n", font_name);
        return;
    }

    size_t canvas_offset = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].occupied;
    struct Font* font = &canvas->fonts[font_i].font;

    // we rely on \0 not being part of the font to advance the cursor at the end of
    // the string one additional time
    log_assert( font->alphabet['\0'] == MAX_FONT_ALPHABET_SIZE );

    float kerning = font->kerning/2.0f;
    bool newline = 0;

    // glyphs that do not exist are not put into the array to be drawn, so we can't use i to
    // compute the array offsets, but must have a seperate counter that only increases if an
    // actual glyph gets added
    uint32_t glyph_counter = 0;
    for( uint32_t i = 0; i <= text_length; i++ ) {
        struct Glyph* glyph = NULL;

        // - this must use MAX_FONT_ALPHABET_SIZE because we can only index it with c, when a
        // glyph does not exist glyph_i is always going to be == MAX_FONT_ALPHABET_SIZE
        static bool glyph_warnings[MAX_FONT_ALPHABET_SIZE] = {0};
        wchar_t c = unicode_text[i];
        wchar_t glyph_i = font->alphabet[c];

        if( c == L'\n' ) {
            // - if c is a newline we just continue with the next line without doing anything
            newline = 1;
            continue;
        } else if( glyph_i < MAX_FONT_GLYPHS ) {
            // - if glyph_i is < MAX_FONT_GLYPHS (which also implies that it should be smaller then
            // MAX_FONT_ALPHABET_SIZE) then that means the font_create function created a glyph and
            // set alphabet[c] to an index into glyphs for the character c, so we can get that glyph
            // and display it
            glyph = &font->glyphs[glyph_i];
        } else if( ! glyph_warnings[c] ) {
            // - if we reach this point, then the text contain a character c, for which no glyph exists,
            // and we want to warn about it once, but only if c is not \0
            if( c == L'\0') {
                log_assert(i == text_length);
            } else {
                log_warn(__FILE__, __LINE__, "font %s does not contain glyph \"%c\"\n", font_name, c);
                glyph_warnings[c] = true;
            }
        }

        // - handle a newline, which is just as simple as resetting x translation to 0.0 and shifting
        // y translation
        if( newline && i > 0 ) {
            float y = 1.0f + font->linespacing;
            cursor_translation[0] = 0.0;
            cursor_translation[1] -= y;
        } else if( ! newline && i > 0 ) {
            float x = 1.0f - kerning;
            cursor_translation[0] += x;
        }
        newline = 0;

        // - append the vertices etc. to the arrays that will be used to draw on the canvas
        if( glyph ) {
            Mat glyph_matrix = {0};
            mat_translate(NULL, cursor_translation, glyph_matrix);
            mat_scale(glyph_matrix, scale, glyph_matrix);
            mat_mul(glyph_matrix, model_matrix, glyph_matrix);

            uint32_t primitive_offset = glyph_counter*4;
            uint32_t vertex_offset = primitive_offset*VERTEX_SIZE;
            uint32_t color_offset = primitive_offset*COLOR_SIZE;
            uint32_t texcoord_offset = primitive_offset*TEXCOORD_SIZE;

            mat_mul_vec( glyph_matrix, (Vec3f){0.0f - kerning, 0.0f, 0.0f}, &vertices[vertex_offset + 0*VERTEX_SIZE] );
            mat_mul_vec( glyph_matrix, (Vec3f){1.0f - kerning, 0.0f, 0.0f}, &vertices[vertex_offset + 1*VERTEX_SIZE] );
            mat_mul_vec( glyph_matrix, (Vec3f){1.0f - kerning, -1.0f, 0.0f}, &vertices[vertex_offset + 2*VERTEX_SIZE] );
            mat_mul_vec( glyph_matrix, (Vec3f){0.0f - kerning, -1.0f, 0.0f}, &vertices[vertex_offset + 3*VERTEX_SIZE] );

            color_copy( color, &colors[color_offset + 0*COLOR_SIZE] );
            color_copy( color, &colors[color_offset + 1*COLOR_SIZE] );
            color_copy( color, &colors[color_offset + 2*COLOR_SIZE] );
            color_copy( color, &colors[color_offset + 3*COLOR_SIZE] );

            float x_step = 1.0f/font->texture.width;
            float y_step = 1.0f/font->texture.height;
            float u1 = x_step * glyph->x;
            float u2 = x_step * (glyph->x + glyph->w);
            float v1 = y_step * glyph->y;
            float v2 = y_step * (glyph->y + glyph->h);
            vec_copy2f( (Vec2f){u1, v2}, &texcoords[texcoord_offset + 0*TEXCOORD_SIZE] );
            vec_copy2f( (Vec2f){u2, v2}, &texcoords[texcoord_offset + 1*TEXCOORD_SIZE] );
            vec_copy2f( (Vec2f){u2, v1}, &texcoords[texcoord_offset + 2*TEXCOORD_SIZE] );
            vec_copy2f( (Vec2f){u1, v1}, &texcoords[texcoord_offset + 3*TEXCOORD_SIZE] );

            uint32_t indices_offset = glyph_counter*6;
            indices[indices_offset+0] = canvas_offset + primitive_offset + 3;
            indices[indices_offset+1] = canvas_offset + primitive_offset + 1;
            indices[indices_offset+2] = canvas_offset + primitive_offset + 0;
            indices[indices_offset+3] = canvas_offset + primitive_offset + 3;
            indices[indices_offset+4] = canvas_offset + primitive_offset + 2;
            indices[indices_offset+5] = canvas_offset + primitive_offset + 1;

            glyph_counter += 1;
        }
    }

    // - finally draw on the canvas
    if( glyph_counter > 0 ) {
        canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX, VERTEX_SIZE, GL_FLOAT, glyph_counter*4, vertices);
        canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_COLOR, COLOR_SIZE, GL_UNSIGNED_BYTE, glyph_counter*4, colors);
        canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_TEXCOORD, TEXCOORD_SIZE, GL_FLOAT, glyph_counter*4, texcoords);
        canvas_append_text(canvas, layer, font_name, projection, glyph_counter*6, indices, 0);
    }

    if( cursor != NULL ) {
        vec_copy4f(cursor_translation, cursor);
    }
}

void text_put_world(struct Canvas* canvas,
                    int32_t layer,
                    Vec4f cursor,
                    const Mat model_matrix,
                    const Color color,
                    float scale,
                    const char* font_name,
                    const wchar_t* unicode_text)
{
    text_put(canvas, layer, cursor, CANVAS_PROJECT_PERSPECTIVE, model_matrix, color, scale, font_name, unicode_text);
}

void text_put_screen(struct Canvas* canvas,
                     int32_t layer,
                     Vec4f cursor,
                     int32_t x, int32_t y,
                     const Color color,
                     float scale,
                     const char* font_name,
                     const wchar_t* unicode_text)
{
    Vec4f translation = {(float)x, (float)-y, 0.0f, 1.0f};
    Mat model_matrix = {0};
    mat_translate(NULL, translation, model_matrix);

    text_put(canvas, layer, cursor, CANVAS_PROJECT_SCREEN, model_matrix, color, scale, font_name, unicode_text);
}

void text_printf(struct Canvas* canvas,
                 int32_t layer,
                 Vec4f cursor,
                 int32_t x, int32_t y,
                 const Color color,
                 float scale,
                 const char* font_name,
                 const wchar_t* format,
                 ...)
{
    va_list args;
    va_start(args, format);

    wchar_t text[MAX_TEXT_PUT_SIZE];
    vswprintf(text, MAX_TEXT_PUT_SIZE, format, args);
    va_end(args);

    text_put_screen(canvas, layer, cursor, x, y, color, scale, font_name, text);
}

double text_show_fps(struct Canvas* canvas,
                     int32_t layer,
                     Vec4f cursor,
                     int32_t x, int32_t y,
                     const Color color,
                     float scale,
                     const char* font_name,
                     double delta)
{

    static size_t frames_done = 0;
    static double old_time = -1;
    static double game_time = 0;
    static double fps = 0;
    static double frame_time = 0;
    static double avg_frame_time_ms = 0;

    game_time += delta;
    frame_time += delta;

    if( old_time < 0 ) {
        old_time = game_time;
    }

    if( (game_time - old_time) >= 0.5 ) {
        avg_frame_time_ms = frame_time*1000 / frames_done;
        frame_time = 0;

        // - this division here should automatically convert from whatever time interval
        // we use in the condition above to seconds
        fps = ((double)frames_done / (game_time - old_time));

        frames_done = 0;
        old_time = game_time;
    }
    frames_done++;

    text_printf(canvas, layer, cursor, x, y, color, scale, font_name, L"FPS: %.1f %.1f\n", fps, avg_frame_time_ms);

    return fps;
}

void text_show_time(struct Canvas* canvas,
                    int32_t layer,
                    Vec4f cursor,
                    int32_t x, int32_t y,
                    const Color color,
                    float scale,
                    const char* font_name,
                    double time)
{
    int32_t hours = 0;
    int32_t minutes = 0;
    int32_t seconds = 0;
    int32_t milliseconds = 0;

    hours = (int)floor(time) / 60 / 60;
    minutes = (int)floor(time) / 60 % 60;
    seconds = (int)floor(time) % 60;
    milliseconds = (int)floor((time - floor(time)) * 1000);

    text_printf(canvas, layer, cursor, x, y, color, scale, font_name, L"TIME:%02d:%02d:%02d.%03d\n", hours, minutes, seconds, milliseconds);
}
