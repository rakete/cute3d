#include "gui_text.h"

void text_put(struct Canvas* canvas, Vec4f cursor, int32_t layer, int32_t projection, const char* font_name, float scale, const Color color, const Mat model_matrix, const wchar_t* unicode_text) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    log_assert( layer >= 0 );
    log_assert( projection >= 0 );
    log_assert( canvas != NULL );
    log_assert( scale > 0.0f );
    log_assert( color != NULL );
    log_assert( model_matrix != NULL );

    size_t text_length = wcslen(unicode_text);
    if( text_length == 0 ) {
        log_warn(stderr, __FILE__, __LINE__, "no text to display\n");
        return;
    }

    Vec4f cursor_translation = {0.0, 0.0, 0.0, 1.0};
    if( cursor != NULL ) {
        vec_copy(cursor, cursor_translation);
    }

    const uint32_t vertex_size = 3;
    const uint32_t color_size = 4;
    const uint32_t texcoord_size = 2;

    // these are too large, not all glyphs will be put into the arrays
    float vertices[text_length * 4 * vertex_size];
    uint8_t colors[text_length * 4 * color_size];
    float texcoords[text_length * 4 * texcoord_size];
    uint32_t indices[text_length * 6];

    int32_t font_i = canvas_find_font(canvas, font_name);
    if( font_i == NUM_CANVAS_FONTS ) {
        log_warn(stderr, __FILE__, __LINE__, "font %s not found\n", font_name);
        return;
    }

    size_t canvas_offset = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;
    struct Font* font = &canvas->fonts[font_i];

    // we rely on \0 not being part of the font to advance the cursor at the end of
    // the string one additional time
    log_assert( font->alphabet['\0'] == false );

    float kerning = font->kerning/2.0f;
    bool newline = 0;

    char ascii_text[text_length + 1];
    size_t ascii_conversion_size = wcstombs(ascii_text, unicode_text, text_length);
    if( ascii_conversion_size >= text_length ) {
        ascii_text[text_length] = '\0';
    }

    // glyphs that do not exist are not put into the array to be drawn, so we can't use i to
    // compute the array offsets, but must have a seperate counter that only increases if an
    // actual glyph gets added
    uint32_t glyph_counter = 0;
    for( uint32_t i = 0; i <= text_length; i++ ) {
        struct Glyph* glyph = NULL;

        static bool glyph_warnings[NUM_FONT_GLYPHS] = {0};
        if( font->unicode ) {
            wchar_t c = unicode_text[i];
            if( c == '\n' ) {
                newline = 1;
                continue;
            } else if( font->alphabet[c] ) {
                glyph = &font->glyphs[c];
            } else if( ! glyph_warnings[c] ) {
                if( c == '\0') {
                    log_assert(i == text_length);
                } else {
                    log_warn(stderr, __FILE__, __LINE__, "font %s does not contain glyph \"%c\"\n", font->name, c);
                    glyph_warnings[c] = true;
                }
            }
        } else {
            char c = ascii_text[i];
            if( c == '\n' ) {
                newline = 1;
                continue;
            } else if( font->alphabet[(int)c] ) {
                glyph = &font->glyphs[(int)c];
            } else if( ! glyph_warnings[(int)c] ) {
                if( c == '\0') {
                    log_assert(i == text_length);
                } else {
                    log_warn(stderr, __FILE__, __LINE__, "font %s does not contain glyph \"%c\"\n", font->name, c);
                    glyph_warnings[(int)c] = true;
                }
            }
        }

        if( newline && i > 0 ) {
            float y = 1.0f + font->linespacing;
            cursor_translation[0] = 0.0;
            cursor_translation[1] -= y;
        } else if( ! newline && i > 0 ) {
            float x = 1.0f - kerning;
            cursor_translation[0] += x;
        }
        newline = 0;

        if( glyph ) {
            Mat glyph_matrix;
            mat_translate(NULL, cursor_translation, glyph_matrix);
            mat_scale(glyph_matrix, scale, glyph_matrix);
            mat_mul(glyph_matrix, model_matrix, glyph_matrix);

            uint32_t primitive_offset = glyph_counter*4;
            uint32_t vertex_offset = primitive_offset*vertex_size;
            uint32_t color_offset = primitive_offset*color_size;
            uint32_t texcoord_offset = primitive_offset*texcoord_size;

            mat_mul_vec3f( glyph_matrix, (Vec3f){0.0 - kerning, 0.0, 0.0}, &vertices[vertex_offset + 0*vertex_size] );
            mat_mul_vec3f( glyph_matrix, (Vec3f){1.0 - kerning, 0.0, 0.0}, &vertices[vertex_offset + 1*vertex_size] );
            mat_mul_vec3f( glyph_matrix, (Vec3f){1.0 - kerning, -1.0, 0.0}, &vertices[vertex_offset + 2*vertex_size] );
            mat_mul_vec3f( glyph_matrix, (Vec3f){0.0 - kerning, -1.0, 0.0}, &vertices[vertex_offset + 3*vertex_size] );

            color_copy( color, &colors[color_offset + 0*color_size] );
            color_copy( color, &colors[color_offset + 1*color_size] );
            color_copy( color, &colors[color_offset + 2*color_size] );
            color_copy( color, &colors[color_offset + 3*color_size] );

            float x_step = 1.0f/font->texture.width;
            float y_step = 1.0f/font->texture.height;
            float u1 = x_step * (glyph->x + 0.0f * glyph->w);
            float u2 = x_step * (glyph->x + 1.0f * glyph->w);
            float v1 = y_step * (glyph->y + 0.0f * glyph->h);
            float v2 = y_step * (glyph->y + 1.0f * glyph->h);
            vec_copy2f( (Vec2f){u1, v1}, &texcoords[texcoord_offset + 0*texcoord_size] );
            vec_copy2f( (Vec2f){u2, v1}, &texcoords[texcoord_offset + 1*texcoord_size] );
            vec_copy2f( (Vec2f){u2, v2}, &texcoords[texcoord_offset + 2*texcoord_size] );
            vec_copy2f( (Vec2f){u1, v2}, &texcoords[texcoord_offset + 3*texcoord_size] );

            uint32_t indices_offset = glyph_counter*6;
            indices[indices_offset+0] = canvas_offset + primitive_offset + 0;
            indices[indices_offset+1] = canvas_offset + primitive_offset + 1;
            indices[indices_offset+2] = canvas_offset + primitive_offset + 2;
            indices[indices_offset+3] = canvas_offset + primitive_offset + 0;
            indices[indices_offset+4] = canvas_offset + primitive_offset + 2;
            indices[indices_offset+5] = canvas_offset + primitive_offset + 3;

            glyph_counter += 1;
        }
    }

    canvas_append_vertices(canvas, vertices, vertex_size, GL_FLOAT, glyph_counter*4, NULL);
    canvas_append_colors(canvas, colors, color_size, GL_UNSIGNED_BYTE, glyph_counter*4, NULL);
    canvas_append_texcoords(canvas, texcoords, texcoord_size, GL_FLOAT, glyph_counter*4);
    canvas_append_text(canvas, layer, projection, font_name, indices, glyph_counter*6, 0);

    if( cursor != NULL ) {
        vec_copy(cursor_translation, cursor);
    }
}

void text_put_world(struct Canvas* canvas, Vec4f cursor, int32_t layer, const char* font_name, float scale, const Color color, const Mat model_matrix, const wchar_t* unicode_text) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    text_put(canvas, cursor, layer, CANVAS_PROJECT_WORLD, font_name, scale, color, model_matrix, unicode_text);
}

void text_put_screen(struct Canvas* canvas, Vec4f cursor, int32_t layer, const char* font_name, float scale, const Color color, int32_t x, int32_t y, const wchar_t* unicode_text) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    Vec translation = {x, -y, 0.0, 1.0};
    Mat model_matrix = {0};
    mat_translate(NULL, translation, model_matrix);

    text_put(canvas, cursor, layer, CANVAS_PROJECT_SCREEN, font_name, scale, color, model_matrix, unicode_text);
}

void text_printf(struct Canvas* canvas, Vec4f cursor, int32_t layer, const char* font_name, float scale, const Color color, int32_t x, int32_t y, const wchar_t* format, ...) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    va_list args;
    va_start(args, format);

    const size_t text_size = 8192;
    wchar_t text[text_size];
    vswprintf(text, text_size, format, args);
    va_end(args);

    text_put_screen(canvas, cursor, layer, font_name, scale, color, x, y, text);
}

double text_show_fps(struct Canvas* canvas, Vec4f cursor, int32_t layer, const char* font_name, float scale, const Color color, int32_t x, int32_t y, double delta) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

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

    text_printf(canvas, cursor, layer, font_name, scale, color, x, y, L"FPS :%.1f\n", fps);

    return fps;
}

void text_show_time(struct Canvas* canvas, Vec4f cursor, int32_t layer, const char* font_name, float scale, const Color color, int32_t x, int32_t y, double time) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    int32_t hours = 0;
    int32_t minutes = 0;
    int32_t seconds = 0;
    int32_t milliseconds = 0;

    hours = (int)floor(time) / 60 / 60;
    minutes = (int)floor(time) / 60 % 60;
    seconds = (int)floor(time) % 60;
    milliseconds = (int)floor((time - floor(time)) * 1000);

    text_printf(canvas, cursor, layer, font_name, scale, color, x, y, L"TIME:%02d:%02d:%02d.%03d\n", hours, minutes, seconds, milliseconds);
}
