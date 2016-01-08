#include "gui_font.h"

void font_create(struct Font* font, const wchar_t* unicode_alphabet, bool unicode, struct Character* symbols, const char* name) {
    int name_length = strlen(name);
    assert( name_length > 0 );
    assert( name_length < 256 );

    strncpy(font->name, name, name_length+1);
    font->unicode = unicode;
    font->kerning = 0.2;
    font->linespacing = 0.2;
    font->color[0] = 1.0;
    font->color[1] = 1.0;
    font->color[2] = 1.0;
    font->color[3] = 1.0;

    //int n = strlen(alphabet);

    unsigned int n = wcslen(unicode_alphabet);
    char ascii_alphabet[n + 1];
    size_t size = wcstombs(ascii_alphabet, unicode_alphabet, n);
    if( size >= n ) {
        ascii_alphabet[n] = '\0';
    }

    int max_h = 0;
    int widths[n];
    for( unsigned int i = 0; i < n; i++ ) {
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
    for( unsigned int i = 0; i < n; i++ ) {
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

    int texture_size = power2 * power2;
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
