#include "gui_text.h"

#define TEXT_VERTEX_SIZE 3
#define TEXT_COLOR_SIZE 4
#define TEXT_TEXCOORD_SIZE 2

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

    size_t canvas_offset = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;
    struct Font* font = &canvas->fonts[font_i];

    // we rely on \0 not being part of the font to advance the cursor at the end of
    // the string one additional time
    log_assert( font->alphabet['\0'] == false );

    float kerning = font->kerning/2.0f;
    bool newline = 0;

    static char ascii_text[MAX_TEXT_PUT_SIZE] = {0};
    size_t ascii_conversion_size = wcstombs(ascii_text, unicode_text, text_length);
    log_assert( ascii_conversion_size < MAX_TEXT_PUT_SIZE );
    log_assert( ascii_conversion_size == text_length );
    ascii_text[text_length] = '\0';

    // glyphs that do not exist are not put into the array to be drawn, so we can't use i to
    // compute the array offsets, but must have a seperate counter that only increases if an
    // actual glyph gets added
    uint32_t glyph_counter = 0;
    for( uint32_t i = 0; i <= text_length; i++ ) {
        struct Glyph* glyph = NULL;

        static bool glyph_warnings[MAX_FONT_GLYPHS] = {0};
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
                    log_warn(__FILE__, __LINE__, "font %s does not contain glyph \"%c\"\n", font->name, c);
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
                } else if( c != ' ' ) {
                    log_warn(__FILE__, __LINE__, "font %s does not contain glyph \"%c\"\n", font->name, c);
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
            Mat glyph_matrix = {0};
            mat_translate(NULL, cursor_translation, glyph_matrix);
            mat_scale(glyph_matrix, scale, glyph_matrix);
            mat_mul(glyph_matrix, model_matrix, glyph_matrix);

            uint32_t primitive_offset = glyph_counter*4;
            uint32_t vertex_offset = primitive_offset*TEXT_VERTEX_SIZE;
            uint32_t color_offset = primitive_offset*TEXT_COLOR_SIZE;
            uint32_t texcoord_offset = primitive_offset*TEXT_TEXCOORD_SIZE;

            mat_mul_vec3f( glyph_matrix, (Vec3f){0.0 - kerning, 0.0, 0.0}, &vertices[vertex_offset + 0*TEXT_VERTEX_SIZE] );
            mat_mul_vec3f( glyph_matrix, (Vec3f){1.0 - kerning, 0.0, 0.0}, &vertices[vertex_offset + 1*TEXT_VERTEX_SIZE] );
            mat_mul_vec3f( glyph_matrix, (Vec3f){1.0 - kerning, -1.0, 0.0}, &vertices[vertex_offset + 2*TEXT_VERTEX_SIZE] );
            mat_mul_vec3f( glyph_matrix, (Vec3f){0.0 - kerning, -1.0, 0.0}, &vertices[vertex_offset + 3*TEXT_VERTEX_SIZE] );

            color_copy( color, &colors[color_offset + 0*TEXT_COLOR_SIZE] );
            color_copy( color, &colors[color_offset + 1*TEXT_COLOR_SIZE] );
            color_copy( color, &colors[color_offset + 2*TEXT_COLOR_SIZE] );
            color_copy( color, &colors[color_offset + 3*TEXT_COLOR_SIZE] );

            float x_step = 1.0f/font->texture.width;
            float y_step = 1.0f/font->texture.height;
            float u1 = x_step * (glyph->x + 0.0f * glyph->w);
            float u2 = x_step * (glyph->x + 1.0f * glyph->w);
            float v1 = y_step * (glyph->y + 0.0f * glyph->h);
            float v2 = y_step * (glyph->y + 1.0f * glyph->h);
            vec_copy2f( (Vec2f){u1, v1}, &texcoords[texcoord_offset + 0*TEXT_TEXCOORD_SIZE] );
            vec_copy2f( (Vec2f){u2, v1}, &texcoords[texcoord_offset + 1*TEXT_TEXCOORD_SIZE] );
            vec_copy2f( (Vec2f){u2, v2}, &texcoords[texcoord_offset + 2*TEXT_TEXCOORD_SIZE] );
            vec_copy2f( (Vec2f){u1, v2}, &texcoords[texcoord_offset + 3*TEXT_TEXCOORD_SIZE] );

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

    if( glyph_counter > 0 ) {
        canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTICES, TEXT_VERTEX_SIZE, GL_FLOAT, glyph_counter*4, vertices);
        canvas_append_attributes(canvas, SHADER_ATTRIBUTE_COLORS, TEXT_COLOR_SIZE, GL_UNSIGNED_BYTE, glyph_counter*4, colors);
        canvas_append_attributes(canvas, SHADER_ATTRIBUTE_TEXCOORDS, TEXT_TEXCOORD_SIZE, GL_FLOAT, glyph_counter*4, texcoords);
        canvas_append_text(canvas, layer, projection, font_name, glyph_counter*6, indices, 0);
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
    text_put(canvas, layer, cursor, CANVAS_PROJECT_WORLD, model_matrix, color, scale, font_name, unicode_text);
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
    Vec4f translation = {x, -y, 0.0, 1.0};
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
