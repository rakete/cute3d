#include "gui_font.h"

void font_create(const wchar_t* unicode_alphabet,
                 size_t symbols_n,
                 struct Character* symbols,
                 size_t palette_n,
                 size_t color_n,
                 uint8_t* palette,
                 const char* name,
                 struct Font* font)
{
    // - first create empty font, this gets rid of the stupid _empty function that I needed once, and then started
    // to implement in all other modules, now this function does this, with NULL arguments
    font->name[0] = '\0';

    for( size_t i = 0; i < MAX_FONT_GLYPHS; i++ ) {
        font->glyphs[i] = (struct Glyph){0, 0, 0, 0};
    }

    for( size_t i = 0; i < MAX_FONT_ALPHABET_SIZE; i++ ) {
        font->alphabet[i] = MAX_FONT_ALPHABET_SIZE;
    }

    font->texture.id = 0;
    font->texture.width = 0;
    font->texture.height = 0;
    font->texture.type = 0;
    font->texture.format = 0;
    font->texture.min_filter = 0;
    font->texture.mag_filter = 0;
    font->texture.wrap_s = 0;
    font->texture.wrap_t = 0;

    shader_create(&font->shader);

    font->kerning = 0.0f;
    font->linespacing = 0.0f;

    // - originally I had two functions, one to create an empty font, and one that actually creates a font,
    // but then I merged them togeter into this one function
    // - if any of the arrays given to this function are NULL, then only create an empty font and return here
    if( unicode_alphabet == NULL || symbols == NULL || palette == NULL || name == NULL ) {
        return;
    }

    // - the real _create starts here
    size_t name_length = strlen(name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    size_t alphabet_len = wcslen(unicode_alphabet);
    log_assert( alphabet_len < MAX_FONT_ALPHABET_SIZE );
    log_assert( alphabet_len < MAX_FONT_GLYPHS );
    log_assert( symbols_n >= alphabet_len );

    log_assert( palette_n > 0 );
    log_assert( color_n == 3 || color_n == 4 );

    font->name[0] = '\0';
    strncat(font->name, name, name_length);

    font->kerning = 0.2f;
    font->linespacing = 0.2f;

    // - I want to to create a font bitmap, where the characters of the alphabet are layed out in rows
    // in such a way that I can easily render one character by using texcoords
    // - currently each row may contain as many characters as can be fitted into it, the character
    // columns do not have to line up
    // - the rows will always have the same height, so I have to loop over the whole alphabet once
    // to find out which character is the highest, then use that max_h height as y offset when progressing
    // to the next row
    int32_t max_h = 0;
    for( int32_t i = 0; i < (int32_t)alphabet_len; i++ ) {
        wchar_t c = unicode_alphabet[i];

        if( symbols[c].h > max_h ) {
            max_h = symbols[c].h;
        }
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

#ifdef CUTE_BUILD_MSVC
    int32_t* rows = _alloca(sizeof(int32_t) * alphabet_len);
    int32_t* row_offsets = _alloca(sizeof(int32_t) * alphabet_len);
#else
    int32_t rows[alphabet_len];
    int32_t row_offsets[alphabet_len];
#endif

    for( int32_t i = 0; i < (int32_t)alphabet_len; i++ ) {
        wchar_t current_c = unicode_alphabet[i];

        row_offsets[i] = row_width;
        rows[i] = row_n;

        int32_t column_height = max_h * ((int32_t)alphabet_len / (i+1)) + ((int32_t)alphabet_len % (i+1) > 0) * max_h;

        row_width += symbols[current_c].w;
        while( power2 < row_width ) {
            power2 *= 2;
        }

        wchar_t next_c = current_c;
        int32_t next_row_width = 0;
        if( i+1 < (int32_t)alphabet_len ) {
            next_c = unicode_alphabet[i+1];
            next_row_width = row_width + symbols[next_c].w;
        }

        if( (i+1) < (int32_t)alphabet_len && column_height <= power2 && next_row_width > power2 ) {
            row_n += 1;
            row_width = 0;
        }
    }
    log_assert( power2 <= 4096 );
    log_assert( power2 > -1 );

    size_t texture_size = (size_t)power2 * (size_t)power2;
    uint8_t* texture = (uint8_t*)calloc( texture_size * 4, sizeof(uint8_t) );
    if( texture != NULL ) {

        for( int32_t i = 0; i < (int32_t)alphabet_len; i++ ) {
            wchar_t c_i = unicode_alphabet[i];
            log_assert( c_i < MAX_FONT_ALPHABET_SIZE );
            font->alphabet[c_i] = i;

            struct Glyph* glyph = &font->glyphs[i];

            int32_t offset_x = row_offsets[i];
            int32_t offset_y = power2 - max_h - rows[i] * max_h;
            int32_t tx;
            int32_t ty;

            glyph->x = offset_x;
            glyph->y = offset_y;
            glyph->w = symbols[c_i].w;
            glyph->h = max_h;

            for( int32_t gy = 0; gy < symbols[c_i].h; gy++ ) {
                for( int32_t gx = 0; gx < symbols[c_i].w; gx++ ) {
                    int32_t pixel_i = gy * symbols[c_i].w + gx;
                    int32_t pixel = symbols[c_i].pixels[pixel_i];

                    log_assert( pixel < (int32_t)palette_n );
                    ColorP* color = &palette[pixel*color_n];

                    tx = offset_x + gx;
                    ty = offset_y + (max_h - 1 - gy); //gy + max_h - symbols[c_i].h;

                    texture[(ty*power2+tx)*4+0] = color[0];
                    texture[(ty*power2+tx)*4+1] = color[1];
                    texture[(ty*power2+tx)*4+2] = color[2];
                    if( color_n == 4 ) {
                        texture[(ty*power2+tx)*4+3] = color[3];
                    } else {
                        // - this hardcodes that in a palette which only has color with 3 components, the first color is
                        // fully transparent, I wanted that so that 0 would always be transparent in my coded font glyphs,
                        // even when I was only using a palette with 3-colors
                        if( pixel == 0 ) {
                            texture[(ty*power2+tx)*4+3] = 0;
                        } else {
                            texture[(ty*power2+tx)*4+3] = 255;
                        }
                    }
                }
            }
        }

        // - the gl code to actualy create the texture and upload the data
        glGenTextures(1, &font->texture.id);
        log_assert( font->texture.id > 0 );

        log_assert( power2 >= 0 );
        font->texture.width = (size_t)power2;
        font->texture.height = (size_t)power2;

        glBindTexture(GL_TEXTURE_2D, font->texture.id);

        font->texture.min_filter = GL_NEAREST;
        font->texture.mag_filter = GL_NEAREST;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, font->texture.min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, font->texture.mag_filter);

        font->texture.wrap_s = GL_CLAMP_TO_EDGE;
        font->texture.wrap_t = GL_CLAMP_TO_EDGE;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, font->texture.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, font->texture.wrap_t);

        font->texture.type = GL_UNSIGNED_BYTE;
        font->texture.format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, font->texture.format, power2, power2, 0, font->texture.format, font->texture.type, texture);

        free(texture);

        shader_create_from_files("shader/default_font.vert", "shader/default_font.frag", "font_shader", &font->shader);
    }
}

void font_texture_filter(struct Font* font, GLint min_filter, GLint mag_filter) {
    glBindTexture(GL_TEXTURE_2D, font->texture.id);

    if( min_filter ) {
        font->texture.min_filter = min_filter;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    }

    if( mag_filter ) {
        font->texture.mag_filter = mag_filter;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}
