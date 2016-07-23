#include "gui_font.h"

void font_create(struct Font* font) {
    // - first create empty font, this gets rid of the stupid _empty function that I needed once, and then started
    // to implement in all other modules, now this function does this, with NULL arguments
    for( size_t i = 0; i < MAX_FONT_GLYPHS; i++ ) {
        font->glyphs[i] = (struct Glyph){0, 0, 0, 0};
    }

    for( size_t i = 0; i < MAX_FONT_ALPHABET_SIZE; i++ ) {
        font->alphabet[i] = MAX_FONT_ALPHABET_SIZE;
    }

    font->texture.id = 0;
    font->texture.width = 0;
    font->texture.height = 0;

    shader_create(&font->shader);

    font->kerning = 0.0f;
    font->linespacing = 0.0f;
}

void font_create_from_characters(const wchar_t* unicode_alphabet,
                                 size_t symbols_n,
                                 struct Character* symbols,
                                 size_t palette_n,
                                 size_t color_n,
                                 uint8_t* palette,
                                 struct Font* font)
{
    for( size_t i = 0; i < MAX_FONT_GLYPHS; i++ ) {
        log_assert( font->glyphs[i].x == 0 );
        log_assert( font->glyphs[i].y == 0 );
        log_assert( font->glyphs[i].w == 0 );
        log_assert( font->glyphs[i].h == 0 );
    }

    for( size_t i = 0; i < MAX_FONT_ALPHABET_SIZE; i++ ) {
        log_assert( font->alphabet[i] == MAX_FONT_ALPHABET_SIZE || font->alphabet[i] == 0 );
    }

    log_assert( font->texture.id == 0 );
    log_assert( font->texture.width == 0 );
    log_assert( font->texture.height == 0 );

    log_assert( font->kerning == 0.0f );
    log_assert( font->linespacing == 0.0f );

    font_create(font);

    // - the real _create starts here
    size_t alphabet_len = wcslen(unicode_alphabet);
    log_assert( alphabet_len < MAX_FONT_ALPHABET_SIZE );
    log_assert( alphabet_len < MAX_FONT_GLYPHS );
    log_assert( symbols_n >= alphabet_len );

    log_assert( palette_n > 0 );
    log_assert( color_n == 3 || color_n == 4 );

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
    // - so I am documenting this after implementing this function, but I think the next section
    // is solving the following problem: I need to know how large I have to make the texture
    // for the font, it will be same width and height, and both width and height will be a power
    // of 2
    // - the table above was created when I originally wrote this, it should help understand
    // the meaning of these variables and how I came up with their computation
    // - so main thing the next loop figures out is the power2 value, which will be the sidelength
    // of the font texture
    int32_t power2 = 64;

    // - along the way I collect row_offsets[i] that I use later when creating the texture, it
    // specifies how much pixels symbol i must be offset from the left texture border to be painted
    // at its correct place
    // - also rows[i], which just specifies which row_n number symbol i should be in
    // - row_width is an accumulator for the row_offsets[], and row_n is an accumulator for
    // the rows[]
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

        // - accumulate this symbols width, increase power2 by doubling until it exceeds row_width
        row_width += symbols[current_c].w;
        while( power2 < row_width ) {
            power2 *= 2;
        }

        // - we compute the next_row_width, then use it below to decide if we continue filling this row, or
        // if we advance to the next row
        wchar_t next_c = current_c;
        int32_t next_row_width = 0;
        if( i+1 < (int32_t)alphabet_len ) {
            next_c = unicode_alphabet[i+1];
            next_row_width = row_width + symbols[next_c].w;
        }
        // - here max_h comes into play by computing what the accumulated column_height for the current row is
        int32_t column_height = max_h * ((int32_t)alphabet_len / (i+1)) + ((int32_t)alphabet_len % (i+1) > 0) * max_h;

        // - when this is not the last iteration, and the pixels we have used up in y direction (column_height) is
        // not yet larger then power2, but the next_row_width is going to exceed the power2 amount of pixels (in x
        // direction), then advance to the next row by increasing row_n by one and resetting the row_width
        // accumulator
        // - its a small wonder this even works so well
        if( (i+1) < (int32_t)alphabet_len && column_height <= power2 && next_row_width > power2 ) {
            row_n += 1;
            row_width = 0;
        }
    }
    log_assert( power2 <= 2048 );
    log_assert( power2 > -1 );

    // - now that we have the row_offsets[] and rows[] figured out, all thats left is putting them to use
    // in the loop that creates the texture[] array that we later upload in as texture
    size_t texture_size = (size_t)power2 * (size_t)power2;
    uint8_t* texture = (uint8_t*)calloc( texture_size * 4, sizeof(uint8_t) );
    if( texture != NULL ) {

        for( int32_t i = 0; i < (int32_t)alphabet_len; i++ ) {
            wchar_t c_i = unicode_alphabet[i];
            log_assert( c_i < MAX_FONT_ALPHABET_SIZE );
            font->alphabet[c_i] = i;

            struct Glyph* glyph = &font->glyphs[i];

            // - offset_x just starts at the left and goes to the right, and its already computed in row_offsets[i]
            // for the row i
            // - offset_y starts at the bottom of the texture and goes up to power2, and since we'd like our font
            // texture to look like you would expect it as a human living in the western hemisphere, we need to
            // put the symbols beginning in the top left corner, going right, making a new line when we hit the
            // right side of the texture, until all symbols are on the texture
            // - so offset_y is computed by taking the y size of the texture (power2), subtracting one row height
            // (max_h) the get the zeroth rows offset pixel, then subtracting all rows height (rows[i] * max_h) for
            // which row symbol i should end up in, to get the offset_y of the symbol i
            // - offset_x and offset_y should form a coordinate tuple pointing of the lower left pixel of where
            // symbol i should go on the font texture
            int32_t offset_x = row_offsets[i];
            int32_t offset_y = power2 - max_h - rows[i] * max_h;

            // - the font texture is not a lined up grid, there are varying glyph widths, so we need to store the
            // x and y coordinates of the lower left pixel, as well as the width and height of the glyph because
            // we need them in the rendering function when computing the texcoords
            glyph->x = offset_x;
            glyph->y = offset_y;
            glyph->w = symbols[c_i].w;
            glyph->h = max_h;

            // - this last nested loop is going through a symbol glyph pixel array (which would be one of the pixel
            // arrays implemented in for example gui_default_font.c), looks up each pixel number in the palette array
            // given to this function, and draws the resulting color into the texture pixel
            int32_t symbol_h = symbols[c_i].h;
            for( int32_t glyph_y = 0; glyph_y < symbols[c_i].h; glyph_y++ ) {
                for( int32_t glyph_x = 0; glyph_x < symbols[c_i].w; glyph_x++ ) {
                    // - the index within the glyph .pixels[] array
                    int32_t pixel_i = glyph_y * symbols[c_i].w + glyph_x;

                    // - get the pixel_v value from the .pixels[] array
                    int32_t pixel_v = symbols[c_i].pixels[pixel_i];

                    // - get the Color associated with the pixel_v value
                    log_assert( pixel_v >= 0 );
                    log_assert( pixel_v < (int32_t)palette_n );
                    ColorP* color = &palette[(size_t)pixel_v*(size_t)color_n];

                    // - again, x goes from left to right, offset_x is x relative to glyph, texture_x is x relative to texture,
                    // we can just add the glyph_x loop var to offset_x to advance the texture_x coordinate
                    // - for texture_y, which has its direction reverset just as offset_y, the loop var glyph_y goes from 0 to
                    // symbols[c_i].h, but we need texture_y to go from offset_y + (symbol_h - 1) to offset_y + 0
                    // - the reason for this reversing lies in how arrays in c work and how textures in opengl work, if we layout
                    // the .pixels[] arrays like in gui_default_font, the 0 row is the top row, but in a texture the 0 row is the
                    // bottom row, therefore we need to 0 row pixels from the glyph .pixels[] array go to (symbol_h - 1) row in
                    // the texture
                    int32_t texture_x = offset_x + glyph_x;
                    int32_t texture_y = offset_y + (symbol_h - 1 - glyph_y); //glyph_y + max_h - symbol_h;

                    texture[(texture_y*power2+texture_x)*4+0] = color[0];
                    texture[(texture_y*power2+texture_x)*4+1] = color[1];
                    texture[(texture_y*power2+texture_x)*4+2] = color[2];
                    if( color_n == 4 ) {
                        texture[(texture_y*power2+texture_x)*4+3] = color[3];
                    } else {
                        // - this hardcodes that in a palette which only has color with 3 components, the first color is
                        // fully transparent, I wanted that so that 0 would always be transparent in my coded font glyphs,
                        // even when I was only using a palette with 3-colors
                        if( pixel_v == 0 ) {
                            texture[(texture_y*power2+texture_x)*4+3] = 0;
                        } else {
                            texture[(texture_y*power2+texture_x)*4+3] = 255;
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, power2, power2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);

        free(texture);

        shader_create_from_files("shader/font_shading.vert", "shader/font_shading.frag", "font_shader", &font->shader);
    }
}
