#include "gui_font.h"

void font_create(struct Font* font, const wchar_t* unicode_alphabet, bool unicode, struct Character* symbols, const char* name) {
    size_t name_length = strlen(name);
    assert( name_length > 0 );
    assert( name_length < 256 );

    strncpy(font->name, name, name_length+1);
    for( int32_t i = 0; i < NUM_FONT_GLYPHS; i++ ) {
        font->alphabet[i] = false;
        font->glyphs[i] = (struct Glyph){0, 0, 0, 0};
    }
    font->unicode = unicode;
    font->kerning = 0.2f;
    font->linespacing = 0.2f;

    size_t alphabet_len = wcslen(unicode_alphabet);
    assert( alphabet_len < INT32_MAX );

    char ascii_alphabet[alphabet_len + 1];
    size_t size = wcstombs(ascii_alphabet, unicode_alphabet, alphabet_len);
    if( size >= alphabet_len ) {
        ascii_alphabet[alphabet_len] = '\0';
    }

    int32_t max_h = 0;
    int32_t widths[alphabet_len];
    for( int32_t i = 0; i < (int32_t)alphabet_len; i++ ) {
        int8_t c = ascii_alphabet[i];
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
    int32_t power2 = 8;
    int32_t row_width = 0;
    int32_t row_n = 0;
    int32_t rows[alphabet_len];
    int32_t row_offsets[alphabet_len];
    for( int32_t i = 0; i < (int32_t)alphabet_len; i++ ) {
        row_offsets[i] = row_width;

        int32_t column_height = max_h * ((int32_t)alphabet_len / (i+1)) + ((int32_t)alphabet_len % (i+1) > 0) * max_h;

        row_width += widths[i];
        while( power2 < row_width ) {
            power2 *= 2;
        }

        int32_t next_row_width = row_width + widths[i+1];
        /* printf("%d %d %d %d %d\n", column_height, row_width, power2, next_row_width, row_n); */
        rows[i] = row_n;
        if( (i+1) < (int32_t)alphabet_len && column_height <= power2 && next_row_width > power2 ) {
            row_n += 1;
            row_width = 0;
        }
    }
    assert( power2 < INT16_MAX );
    assert( power2 > -1 );

    size_t texture_size = (size_t)power2 * (size_t)power2;
    float* texture = (float*)calloc( texture_size * 4,  sizeof(float) );
    if( texture != NULL ) {

        int8_t k = ascii_alphabet[0];
        struct Character c = symbols[k];
        for( int32_t gy = 0; gy < c.h; gy++ ) {
            for( int32_t gx = 0; gx < c.w; gx++ ) {
                texture[(gy*power2+gx)*4+0] = 1.0f * c.pixels[gy*c.w+gx];
                texture[(gy*power2+gx)*4+1] = 1.0f * c.pixels[gy*c.w+gx];
                texture[(gy*power2+gx)*4+2] = 1.0f * c.pixels[gy*c.w+gx];
                texture[(gy*power2+gx)*4+3] = 1.0f * c.pixels[gy*c.w+gx];
            }
        }

        for( int32_t i = 0; i < (int32_t)alphabet_len; i++ ) {
            int8_t c = ascii_alphabet[i];
            font->alphabet[c] = true;

            struct Glyph* glyph = &font->glyphs[c];

            int32_t offset_x = row_offsets[i];
            int32_t offset_y = rows[i] * max_h;
            int32_t tx;
            int32_t ty;

            glyph->x = offset_x;
            glyph->y = offset_y;
            glyph->w = symbols[c].w;
            glyph->h = max_h;

            for( int32_t gy = 0; gy < symbols[c].h; gy++ ) {
                for( int32_t gx = 0; gx < symbols[c].w; gx++ ) {
                    int32_t pixel = symbols[c].pixels[gy * symbols[c].w + gx];
                    tx = offset_x + gx;
                    ty = offset_y + gy + max_h - symbols[c].h;

                    texture[(ty*power2+tx)*4+0] = 1.0f * pixel;
                    texture[(ty*power2+tx)*4+1] = 1.0f * pixel;
                    texture[(ty*power2+tx)*4+2] = 1.0f * pixel;
                    texture[(ty*power2+tx)*4+3] = 1.0f * pixel;
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
                  uniform vec4 diffuse_color;
                  out vec4 frag_color;
                  out vec2 frag_texcoord;
                  void main() {
                      gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex,1.0);
                      frag_color = diffuse_color;
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

        shader_create_from_sources(vertex_source, fragment_source, "font_shader", &font->shader);
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
