#include "text.h"

int font_registry(enum FontOp op, int id, struct Font** font) {
    static struct Font* registry[NUM_FONTS];
    static short reserved_slots[NUM_FONTS] = {0};

    if( ! reserved_slots[0] ) {
        struct Character symbols[256];
        symbols['A'] = char_A();
        symbols['B'] = char_B();
        symbols['C'] = char_C();
        symbols['D'] = char_D();
        symbols['E'] = char_E();
        symbols['F'] = char_F();
        symbols['G'] = char_G();
        symbols['H'] = char_H();
        symbols['I'] = char_I();
        symbols['J'] = char_J();
        symbols['K'] = char_K();
        symbols['L'] = char_L();
        symbols['M'] = char_M();
        symbols['N'] = char_N();
        symbols['O'] = char_O();
        symbols['P'] = char_P();
        symbols['Q'] = char_Q();
        symbols['R'] = char_R();
        symbols['S'] = char_S();
        symbols['T'] = char_T();
        symbols['U'] = char_U();
        symbols['V'] = char_V();
        symbols['W'] = char_W();
        symbols['X'] = char_X();
        symbols['Y'] = char_Y();
        symbols['Z'] = char_Z();
    
        symbols['a'] = char_a();
        symbols['b'] = char_b();
        symbols['c'] = char_c();
        symbols['d'] = char_d();
        symbols['e'] = char_e();
        symbols['f'] = char_f();
        symbols['g'] = char_g();
        symbols['h'] = char_h();
        symbols['i'] = char_i();
        symbols['j'] = char_j();
        symbols['k'] = char_k();
        symbols['l'] = char_l();
        symbols['m'] = char_m();
        symbols['n'] = char_n();
        symbols['o'] = char_o();
        symbols['p'] = char_p();
        symbols['q'] = char_q();
        symbols['r'] = char_r();
        symbols['s'] = char_s();
        symbols['t'] = char_t();
        symbols['u'] = char_u();
        symbols['v'] = char_v();
        symbols['w'] = char_w();
        symbols['x'] = char_x();
        symbols['y'] = char_y();
        symbols['z'] = char_z();
        
        registry[0] = font_allocate_ascii("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", symbols);
        reserved_slots[0] = 1;
    }

    switch( op ) {
        case NewFont: {
            if( id < 0 && font ) {
                font_delete( registry[0] );
                registry[0] = *font;
                return 0;
            }
            int slot = -1;
            for( int i = 1; i < NUM_FONTS && slot < 1 && font; i++ ) {
                if( ! reserved_slots[i] ) {
                    reserved_slots[i] = 1;
                    registry[i] = *font;
                    slot = i;
                }
            }
            return slot;
            break;
        }
        case CloneFont: {
            break;
        }
        case FindFont: {
            if( reserved_slots[id] ) {
                *font = registry[id];
                return id;
            }
            font = &registry[0];
            return 0;
            break;
        }
        case DeleteFont: {
            if( id < -1 ) {
                for( int i = 0; i < NUM_FONTS; i++ ) {
                    reserved_slots[i] = 0;
                    font_delete( registry[i] );
                    return 0;
                }
            } else if( reserved_slots[id] && id > 0 ) {
                reserved_slots[id] = 0;
                font_delete(registry[id]);
                return id;
            }
            return -1;
            break;
        }
    }
}

struct Font* font_allocate_ascii(const char* alphabet, struct Character* symbols) {
    struct Font* font = calloc( 1, sizeof(struct Font) );
    font->heap.size = 256;
    font->heap.glyphs = calloc( 256, sizeof(struct Glyph) );
    font->heap.alphabet = calloc( 256, sizeof(short) );
    font->encoding.unicode = 0;
    font->encoding.size = sizeof(char);
    font->kerning = 0.0;
    font->size = 0.1;
    font->color[0] = 1.0;
    font->color[1] = 1.0;
    font->color[2] = 1.0;
    font->color[3] = 1.0;
    
    int n = strlen(alphabet);

    int max_h = 0;
    int widths[n];
    for( int i = 0; i < n; i++ ) {
        char c = alphabet[i];
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

        struct Character c = symbols[alphabet[0]];
        for( int gy = 0; gy < c.h; gy++ ) {
            for( int gx = 0; gx < c.w; gx++ ) {
                texture[(gy*power2+gx)*4+0] = 1.0 * c.pixels[gy*c.w+gx];
                texture[(gy*power2+gx)*4+1] = 1.0 * c.pixels[gy*c.w+gx];
                texture[(gy*power2+gx)*4+2] = 1.0 * c.pixels[gy*c.w+gx];
                texture[(gy*power2+gx)*4+3] = 1.0 * c.pixels[gy*c.w+gx];
            }
        }
    
        for( int i = 0; i < n; i++ ) {
            char c = alphabet[i];
            font->heap.alphabet[c] = 1;
            
            struct Glyph* glyph = &font->heap.glyphs[c];

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


        if( ! font->shader.program ) {
            font->shader.program = make_program(vertex_source, fragment_source);
        }

        return font;
    }

    return NULL;
}

void font_delete(struct Font* font) {
    glDeleteTextures(1,&font->texture.id);
    
    free(font->heap.glyphs);
    free(font);
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

static void upload_buffer(void* data, GLsizei bytes, GLuint* id, GLenum usage) {
}

static void attach_attrib(const char* location_name, GLuint program, int components, GLenum type) {
    GLint position = glGetAttribLocation(program, location_name);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, components, type, GL_FALSE, 0, 0);
}

void text_render(const wchar_t* text, const struct Font* font, const Matrix projection_matrix, const Matrix view_matrix, const Matrix model_matrix) {

    static GLuint quad = 0;
    static GLuint vertices_id = 0;
    static GLuint texcoords_id = 0;
    static GLuint elements_id = 0;

    GLfloat vertices[6*3] =
        { -1.0, 1.0, 0.0,
          1.0, 1.0, 0.0,
          1.0, -1.0, 0.0,
          1.0, -1.0, 0.0,
          -1.0, -1.0, 0.0,
          -1.0, 1.0, 0.0 };
    GLfloat texcoords[6*2] =
        { 0.0, 1.0,
          1.0, 1.0,
          1.0, 0.0,
          1.0, 0.0,
          0.0, 0.0,
          0.0, 1.0 };
    GLuint elements[6] =
        { 0, 1, 2,
          3, 4, 5 };

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
        glUniform3f(color_loc, 0.0, 0.0, 1.0);

        GLint projection_loc = glGetUniformLocation(font->shader.program, "projection_matrix");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);
        
        GLint view_loc = glGetUniformLocation(font->shader.program, "view_matrix");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);

        GLint diffuse_loc = glGetUniformLocation(font->shader.program, "diffuse");

        /* printf("color_loc: %d\n", color_loc); */
        /* printf("normal_loc: %d\n", normal_loc); */
        /* printf("projection_loc: %d\n", projection_loc); */
        /* printf("view_loc: %d\n", view_loc); */
        /* printf("diffuse_loc: %d\n", diffuse_loc); */

        if( diffuse_loc > -1 ) {
            glUniform1i(diffuse_loc, 0);

            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, font->texture.id);

            GLint glyph_loc = glGetUniformLocation(font->shader.program, "glyph");
            GLint offset_loc = glGetUniformLocation(font->shader.program, "offset");
            GLint model_loc = glGetUniformLocation(font->shader.program, "model_matrix");
            if( glyph_loc > -1 ) {
                /* printf("glyph_loc: %d\n", glyph_loc); */
                /* printf("offset_loc: %d\n", offset_loc); */
                /* printf("model_loc: %d\n", model_loc); */

                int length = wcslen(text);
                char ascii[length + 1];
                size_t size = wcstombs(ascii, text, length);
                if( size >= length ) {
                    ascii[length] = '\0';
                }

                Matrix offset_matrix;
                matrix_identity(offset_matrix);
                for( int i = 0; i < length; i++ ) {

                    struct Glyph* glyph = NULL;
                    if( font->encoding.unicode ) {
                        wchar_t c = text[i];
                        if( font->heap.alphabet[c] ) {
                            glyph = &font->heap.glyphs[c];
                        }
                    } else {
                        char c = ascii[i];
                        if( font->heap.alphabet[c] ) {
                            glyph = &font->heap.glyphs[c];
                        }
                    }

                    matrix_translate(offset_matrix, (Vec){ 1.5, 0.0, 0.0, 1.0 }, offset_matrix);
                    Matrix glyph_matrix;
                    matrix_multiply(model_matrix, offset_matrix, glyph_matrix);

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

void text_screen(const wchar_t* text, const struct Font* font, const Matrix projection_matrix, const Matrix view_matrix, int x, int y) {
}

