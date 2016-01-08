#include "gui_text.h"

void text_put_world(struct Canvas* canvas, int layer, const char* font_name, float scale, const Color color, const wchar_t* text, Mat model_matrix) {
    unsigned int text_length = wcslen(text);
    char ascii[text_length + 1];
    size_t textsize = wcstombs(ascii, text, text_length);
    if( textsize >= text_length ) {
        ascii[text_length] = '\0';
    }

    int font_i = canvas_find_font(canvas, font_name);
    if( font_i == NUM_CANVAS_FONTS ) {
        return;
    }

    struct Font* font = &canvas->fonts[font_i];
    int canvas_offset = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;

    float vertices[text_length * 6 * 3];
    float colors[text_length * 6 * 4];
    float texcoords[text_length * 6 * 2];
    unsigned int indices[text_length * 6];

    Vec4f cursor_translation = {0.0,0.0,0.0};
    bool newline = 0;
    for( unsigned int i = 0; i < text_length; i++ ) {
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
        mat_translate(NULL, cursor_translation, glyph_matrix);
        mat_scale(glyph_matrix, scale, glyph_matrix);
        mat_mul(glyph_matrix, model_matrix, glyph_matrix);

        int attributes_offset = i*4;
        int vertex_size = 3;
        int color_size = 4;
        int texcoord_size = 2;
        mat_mul_vec3f( glyph_matrix, (Vec3f){-0.5, 0.5, 0.0}, vertices + attributes_offset*vertex_size + 0*vertex_size );
        mat_mul_vec3f( glyph_matrix, (Vec3f){0.5, 0.5, 0.0}, vertices + attributes_offset*vertex_size + 1*vertex_size );
        mat_mul_vec3f( glyph_matrix, (Vec3f){0.5, -0.5, 0.0}, vertices + attributes_offset*vertex_size + 2*vertex_size );
        mat_mul_vec3f( glyph_matrix, (Vec3f){-0.5, -0.5, 0.0}, vertices + attributes_offset*vertex_size + 4*vertex_size );

        vec_copy( color, colors + attributes_offset*color_size + 0*color_size );
        vec_copy( color, colors + attributes_offset*color_size + 1*color_size );
        vec_copy( color, colors + attributes_offset*color_size + 2*color_size );
        vec_copy( color, colors + attributes_offset*color_size + 3*color_size );

        float x_step = 1.0/font->texture.width;
        float y_step = 1.0/font->texture.height;
        float u1 = x_step * (glyph->x + 0.0 * glyph->w);
        float u2 = x_step * (glyph->x + 1.0 * glyph->w);
        float v1 = y_step * (glyph->y + 0.0 * glyph->h);
        float v2 = y_step * (glyph->y + 1.0 * glyph->h);
        vec_copy2f( (Vec2f){u1, v1}, texcoords + attributes_offset*texcoord_size + 0*texcoord_size );
        vec_copy2f( (Vec2f){u2, v1}, texcoords + attributes_offset*texcoord_size + 1*texcoord_size );
        vec_copy2f( (Vec2f){u2, v2}, texcoords + attributes_offset*texcoord_size + 2*texcoord_size );
        vec_copy2f( (Vec2f){u1, v2}, texcoords + attributes_offset*texcoord_size + 4*texcoord_size );

        int indices_offset = i*6;
        indices[indices_offset+0] = canvas_offset + indices_offset + 0;
        indices[indices_offset+1] = canvas_offset + indices_offset + 2;
        indices[indices_offset+2] = canvas_offset + indices_offset + 1;
        indices[indices_offset+3] = canvas_offset + indices_offset + 2;
        indices[indices_offset+4] = canvas_offset + indices_offset + 3;
        indices[indices_offset+5] = canvas_offset + indices_offset + 0;
    }

    canvas_append_vertices(canvas, vertices, 3, GL_FLOAT, text_length*4, NULL);
    canvas_append_colors(canvas, colors, 4, GL_FLOAT, text_length*4, NULL);
    canvas_append_texcoords(canvas, texcoords, 2, GL_FLOAT, text_length*4);
    canvas_append_text(canvas, layer, 0, font_name, indices, text_length*6, 0);
}

void text_put_screen(struct Canvas* canvas, int layer, const char* font_name, float scale, const Color color, const wchar_t* text, int x, int y) {
    /* camera.type = CAMERA_ORTHOGRAPHIC; */
    /* Mat ortho_projection, ortho_view; */
    /* camera_matrices(&camera, ortho_projection, ortho_view); */

    Mat text_matrix;
    mat_identity(text_matrix);

    /* Vec translation; */
    /* translation[0] = camera.frustum.left + fabs(camera.frustum.left - camera.frustum.right)/(float)camera.screen.width * (float)x; */
    /* translation[1] = camera.frustum.top - fabs(camera.frustum.top - camera.frustum.bottom)/(float)camera.screen.height * (float)y; */
    /* translation[2] = 0.0; */
    /* translation[3] = 1.0; */
    /* mat_translate(text_matrix, translation, text_matrix); */

    /* mat_rotate(text_matrix, camera.pivot.orientation, text_matrix); */

    // multiplying with zNear fixes text being too small or too big when
    // zNear is set to something other then 1.0
    /* float scale = (float)size / (float)camera.screen.height * camera.frustum.zNear; */

    glDisable(GL_DEPTH_TEST);
    /* text_put(text, font, scale, ortho_projection, ortho_view, text_matrix); */
    glEnable(GL_DEPTH_TEST);
}
