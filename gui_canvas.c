#include "gui_canvas.h"

struct Canvas global_dynamic_canvas = {0};
struct Canvas global_static_canvas = {0};

int32_t init_canvas(int32_t width, int32_t height) {
    canvas_create("global_dynamic_canvas", width, height, &global_dynamic_canvas);
    canvas_create("global_static_canvas", width, height, &global_static_canvas);

    return 0;
}

void canvas_create(const char* name, int32_t width, int32_t height, struct Canvas* canvas) {
    log_assert( canvas != NULL );
    log_assert( strlen(name) > 0 );
    log_assert( strlen(name) < 256 );

    canvas->name[0] = '\0';
    strncat(canvas->name, name, strlen(name));

    canvas->width = width;
    canvas->height = height;

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        canvas->components[i].size = 0;
        if( i == SHADER_ATTRIBUTE_VERTEX_COLOR ) {
            canvas->components[i].type = GL_UNSIGNED_BYTE;
        } else {
            canvas->components[i].type = GL_FLOAT;
        }
        canvas->components[i].bytes = 0;

        canvas->attributes[i].array = NULL;
        canvas->attributes[i].occupied = 0;
        canvas->attributes[i].capacity = 0;

        canvas->buffer[i].id = 0;
        canvas->buffer[i].occupied = 0;
        canvas->buffer[i].capacity = 0;
        canvas->buffer[i].usage = GL_STREAM_READ;
    }

    for( int32_t i = 0; i < MAX_CANVAS_SHADER; i++ ) {
        shader_create(&canvas->shaders[i].shader);
        canvas->shaders[i].name[0] = '\0';
    }

    for( int32_t i = 0; i < MAX_CANVAS_FONTS; i++ ) {
        font_create(&canvas->fonts[i].font);
        canvas->fonts[i].name[0] = '\0';
    }

    for( int32_t i = 0; i < MAX_CANVAS_TEXTURES; i++ ) {
        for( int32_t j = 0; j < MAX_SHADER_SAMPLER; j++ ) {
            texture_create(&canvas->textures[i].sampler[j]);
        }
        canvas->textures[i].name[0] = '\0';
    }

    for( int32_t i = 0; i < MAX_CANVAS_LAYERS; i++ ) {
        for( int32_t j = 0; j < MAX_CANVAS_TEXTURES+1; j++ ) {
            for( int32_t k = 0; k < MAX_CANVAS_SHADER; k++ ) {
                for( int32_t l = 0; l < MAX_CANVAS_PROJECTIONS; l++ ) {
                    for( int32_t m = 0; m < MAX_CANVAS_PRIMITIVES; m++ ) {
                        canvas->layer[i].indices[j][k][l][m].array = NULL;
                        canvas->layer[i].indices[j][k][l][m].id = 0;
                        canvas->layer[i].indices[j][k][l][m].occupied = 0;
                        canvas->layer[i].indices[j][k][l][m].capacity = 0;
                    }
                }
            }
        }


        for( int32_t j = 0; j < MAX_CANVAS_FONTS; j++ ) {
            for( int32_t k = 0; k < MAX_CANVAS_PROJECTIONS; k++ ) {
                canvas->layer[i].text[j][k].array = NULL;
                canvas->layer[i].text[j][k].id = 0;
                canvas->layer[i].text[j][k].occupied = 0;
                canvas->layer[i].text[j][k].capacity = 0;
            }
        }

        vec_copy4f((Vec4f){0, 0, 0, 1}, canvas->layer[i].cursor);
    }

#ifndef CUTE_BUILD_ES2
    canvas->vao = 0;
#endif

    // canvas_create
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_VERTEX_TEXCOORD, 2, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_NEXT_VERTEX, 3, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_PREV_VERTEX, 3, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_LINE_THICKNESS, 1, GL_FLOAT);

    log_info(__FILE__, __LINE__, "creating shaders for canvas: %s\n", name);
    log_indent(1);

    struct Shader shader = {0};
    shader_create(&shader);
    shader_make_program(&shader, SHADER_CANVAS_NAMES, "default_shader");
    log_assert( canvas_add_shader(canvas, "default_shader", &shader) < MAX_CANVAS_SHADER );

    struct Character symbols[256] = {0};
    default_font_create(symbols);

    struct Font font = {0};
    font_create_from_characters(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;+-*/=()[]{}", 256, symbols, 9, 3, global_default_font_palette, &font);

    log_assert( canvas_add_font(canvas, "default_font", &font) < MAX_CANVAS_FONTS );
    log_indent(-1);
}

void canvas_destroy(struct Canvas* canvas) {
    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        if( canvas->attributes[i].array ) {
            free(canvas->attributes[i].array);
        }
    }

    for( int32_t i = 0; i < MAX_CANVAS_LAYERS; i++ ) {
        for( int32_t j = 0; j < MAX_CANVAS_TEXTURES+1; j++ ) {
            for( int32_t k = 0; k < MAX_CANVAS_SHADER; k++ ) {
                for( int32_t l = 0; l < MAX_CANVAS_PROJECTIONS; l++ ) {
                    for( int32_t m = 0; m < MAX_CANVAS_PRIMITIVES; m++ ) {
                        if( canvas->layer[i].indices[j][k][l][m].array ) {
                            free(canvas->layer[i].indices[j][k][l][m].array);
                        }
                    }
                }
            }
        }

        for( int32_t j = 0; j < MAX_CANVAS_FONTS; j++ ) {
            for( int32_t k = 0; k < MAX_CANVAS_PROJECTIONS; k++ ) {
                if( canvas->layer[i].text[j][k].array ) {
                    free(canvas->layer[i].text[j][k].array);
                }
            }
        }
    }
}

void canvas_add_attribute(struct Canvas* canvas, int32_t added_attribute, uint32_t size, GLenum type) {
    log_assert( canvas != NULL );
    log_assert( added_attribute >= 0 && added_attribute < MAX_SHADER_ATTRIBUTES );
    log_assert( size > 0 );

    if( canvas->components[added_attribute].size <= 0 ) {

        canvas->components[added_attribute].size = size;
        canvas->components[added_attribute].type = type;
        uint32_t bytes = (uint32_t)ogl_sizeof_type(type);
        canvas->components[added_attribute].bytes = bytes;

        // if we add an attribute to a canvas that has already vertices allocated and occupied
        // we just allocate and zero the new attributes overlapping with those vertices
        //
        // next time the user calls clear the occupied will be reset and the buffer is used
        // just as any other, so this should just handle a very rare edge case
        size_t vertices_capacity = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].capacity;
        size_t vertices_occupied = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].occupied;
        if( vertices_capacity > 0 ) {
            // because this should be rare, I'll warn about it
            log_warn(__FILE__, __LINE__, "vertices already allocated when adding attribute %d\n", added_attribute);
            canvas->attributes[added_attribute].array = calloc(vertices_capacity * size, bytes);
            canvas->attributes[added_attribute].capacity = vertices_capacity;
            canvas->attributes[added_attribute].occupied = vertices_occupied;
        }
    }
}

int32_t canvas_add_shader(struct Canvas* canvas, const char* shader_name, const struct Shader* shader) {
    log_assert( shader != NULL );
    size_t name_length = strlen(shader_name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    // - look at all shaders to find a slot without a name, that slot will then be filled with the shader
    // we want to add by the memcpy below, if we find a shader with the same name as the one to be added we
    // return early
    // - before this was static I warned about when a shader was already added, I made this static and removed
    // the warning
    // - the shaders are added one after the other, and can not be removed, so I make this static because we can
    // assume that all shaders before shader_i will _always_ be set
    // - I removed the static again, I want this function to return the index of the shader if it already exists,
    // for that I have to check all shaders
    int32_t shader_i = 0;
    while( shader_i < MAX_CANVAS_SHADER && strlen(canvas->shaders[shader_i].name) != 0 ) {
        if( strncmp(canvas->shaders[shader_i].name, shader_name, 256) == 0 ) {
            return shader_i;
        }

        shader_i += 1;
    }

    // - when we have not found any slot with an empty name it means we can not add the shader to the
    // canvas and we return MAX_CANVAS_SHADER to indicate that
    if( shader_i == MAX_CANVAS_SHADER ) {
        log_warn(__FILE__, __LINE__, "no more space available in canvas for adding shader \"%s\"\n", shader_name);
        return MAX_CANVAS_SHADER;
    }

    // - copy the shader into the shader_i slot of the canvas
    memcpy(&canvas->shaders[shader_i].name, shader_name, name_length+1);
    memcpy(&canvas->shaders[shader_i].shader, shader, sizeof(struct Shader));

    return shader_i;
}

int32_t canvas_find_shader(const struct Canvas* canvas, const char* shader_name) {
    size_t name_length = strlen(shader_name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    // - repeated calls with the same shader will not go through the shader array every time, I keep the last
    // found shader index in check_first and try it first before looping throug the array
    static int32_t check_first = -1;
    if( check_first > -1 &&
        check_first < MAX_CANVAS_SHADER &&
        strncmp(canvas->shaders[check_first].name, shader_name, 256) == 0 )
    {
        return check_first;
    }

    // - loop through the array of shaders and compare names, until there is a shader with a matching name, if
    // there is shader with an empty name we can return early because that means the shader will not be found
    int32_t shader_i = 0;
    while( shader_i < MAX_CANVAS_SHADER && strncmp(canvas->shaders[shader_i].name, shader_name, 256) != 0 )
    {
        if( strlen(canvas->shaders[shader_i].name) == 0 ) {
            return MAX_CANVAS_SHADER;
        }
        shader_i += 1;
    }

    check_first = shader_i;

    return shader_i;
}

int32_t canvas_add_font(struct Canvas* canvas, const char* font_name, const struct Font* font) {
    log_assert( font != NULL );
    size_t name_length = strlen(font_name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    int32_t font_i = 0;
    while( font_i < MAX_CANVAS_FONTS && strlen(canvas->fonts[font_i].name) != 0 ) {
        if( strncmp(canvas->fonts[font_i].name, font_name, 256) == 0 ) {
            return font_i;
        }

        font_i += 1;
    }

    if( font_i == MAX_CANVAS_FONTS ) {
        return MAX_CANVAS_FONTS;
    }

    strncat(canvas->fonts[font_i].name, font_name, name_length);
    memcpy(&canvas->fonts[font_i].font, font, sizeof(struct Font));

    return font_i;
}

int32_t canvas_find_font(const struct Canvas* canvas, const char* font_name) {
    size_t name_length = strlen(font_name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    static int32_t check_first = 0;
    if( check_first > 0 &&
        check_first < MAX_CANVAS_FONTS &&
        strncmp(canvas->fonts[check_first].name, font_name, 256) == 0 )
    {
        return check_first;
    }

    int32_t font_i = 0;
    while( font_i < MAX_CANVAS_FONTS && strncmp(canvas->fonts[font_i].name, font_name, 256) != 0 ) {
        if( strlen(canvas->fonts[font_i].name) == 0 ) {
            return MAX_CANVAS_FONTS;
        }
        font_i += 1;
    }

    if( font_i == MAX_CANVAS_FONTS && check_first == 0 ) {
        log_warn(__FILE__, __LINE__, "font \"%s\" not found\n", font_name);
    }

    check_first = font_i;

    return font_i;
}

int32_t canvas_add_texture(struct Canvas* canvas, int32_t sampler, const char* texture_name, const struct Texture* texture) {
    log_assert( texture != NULL );
    size_t name_length = strlen(texture_name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    int32_t sampler_i = sampler % MAX_SHADER_TEXTURE_UNITS;
    log_assert( sampler_i >= 0 );
    log_assert( sampler_i < MAX_SHADER_SAMPLER );

    int32_t texture_i = 0;
    while( texture_i < MAX_CANVAS_TEXTURES && strlen(canvas->textures[texture_i].name) != 0 ) {
        if( strncmp(canvas->textures[texture_i].name, texture_name, 256) == 0 ) {
            return texture_i;
        }

        texture_i += 1;
    }

    if( texture_i == MAX_CANVAS_TEXTURES ) {
        return MAX_CANVAS_TEXTURES;
    }

    strncat(canvas->textures[texture_i].name, texture_name, name_length);
    memcpy(&canvas->textures[texture_i].sampler[sampler_i], texture, sizeof(struct Texture));

    return texture_i;
}

int32_t canvas_find_texture(const struct Canvas* canvas, const char* texture_name) {
    size_t name_length = strlen(texture_name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    static int32_t check_first = 0;
    if( check_first > 0 &&
        check_first < MAX_CANVAS_TEXTURES &&
        strncmp(canvas->textures[check_first].name, texture_name, 256) == 0 )
    {
        return check_first;
    }

    int32_t texture_i = 0;
    while( texture_i < MAX_CANVAS_TEXTURES && strncmp(canvas->textures[texture_i].name, texture_name, 256) != 0 ) {
        if( strlen(canvas->textures[texture_i].name) == 0 ) {
            return MAX_CANVAS_TEXTURES;
        }
        texture_i += 1;
    }

    if( texture_i == MAX_CANVAS_TEXTURES && check_first == 0 ) {
        log_warn(__FILE__, __LINE__, "texture \"%s\" not found\n", texture_name);
    }

    check_first = texture_i;

    return texture_i;
}

size_t canvas_alloc_attributes(struct Canvas* canvas, uint32_t attribute_i, size_t n) {
    log_assert( canvas != NULL );
    log_assert( attribute_i < MAX_SHADER_ATTRIBUTES );

    if( n == 0 ) {
        return 0;
    }

    size_t old_capacity = canvas->attributes[attribute_i].capacity;
    log_assert( INT32_MAX - n > old_capacity );
    size_t new_capacity = old_capacity + n;

    uint32_t size = canvas->components[attribute_i].size;

    log_assert( size > 0 );
    float* old_array_pointer = canvas->attributes[attribute_i].array;
    float* new_array_pointer = (float*)realloc(old_array_pointer, new_capacity * size * sizeof(float));

    log_assert( new_array_pointer != NULL );
    canvas->attributes[attribute_i].array = new_array_pointer;
    canvas->attributes[attribute_i].capacity = new_capacity;

    return n;
}

size_t canvas_alloc_indices(struct Canvas* canvas, int32_t layer_i, int32_t texture_i, const char* shader_name, int32_t projection_i, GLenum primitive_type, size_t n) {
    log_assert( canvas != NULL );
    log_assert( layer_i >= 0 );
    log_assert( projection_i >= 0 );
    log_assert( primitive_type == GL_LINES || primitive_type == GL_TRIANGLES );

    if( n == 0 ) {
        return 0;
    }

    int32_t shader_i = canvas_find_shader(canvas, shader_name);
    if( shader_i == MAX_CANVAS_SHADER ) {
        shader_i = canvas_find_shader(canvas, "default_shader");
    }

    if( shader_i == MAX_CANVAS_SHADER ) {
        log_fail(__FILE__, __LINE__, "no shader could be found in canvas when trying to allocate\n");
        return 0;
    }

    int32_t primitive_i = CANVAS_TRIANGLES;
    if( primitive_type == GL_LINES ) {
        primitive_i = CANVAS_LINES;
    }

    size_t old_capacity = canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].capacity;
    log_assert( INT32_MAX - n > old_capacity );
    size_t new_capacity = old_capacity + n;

    GLuint* old_array_pointer = canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].array;
    GLuint* new_array_pointer = realloc(old_array_pointer, new_capacity * sizeof(GLuint));

    log_assert( new_array_pointer != NULL );
    canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].array = new_array_pointer;
    canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].capacity = new_capacity;

    return n;
}

size_t canvas_alloc_text(struct Canvas* canvas, int32_t layer_i, const char* font_name, int32_t projection_i, size_t n) {
    log_assert( canvas != NULL );
    log_assert( layer_i >= 0 );

    if( n == 0 ) {
        return 0;
    }

    int32_t font_i = canvas_find_font(canvas, font_name);
    if( font_i == MAX_CANVAS_FONTS ) {
        return 0;
    }

    size_t old_capacity = canvas->layer[layer_i].text[font_i][projection_i].capacity;
    log_assert( INT32_MAX - n > old_capacity );
    size_t new_capacity = old_capacity + n;

    GLuint* old_array_pointer = canvas->layer[layer_i].text[font_i][projection_i].array;
    GLuint* new_array_pointer = realloc(old_array_pointer, new_capacity * sizeof(GLuint));

    log_assert( new_array_pointer != NULL );
    canvas->layer[layer_i].text[font_i][projection_i].array = new_array_pointer;
    canvas->layer[layer_i].text[font_i][projection_i].capacity = new_capacity;

    return n;
}

void canvas_clear(struct Canvas* canvas) {
    log_assert( canvas != NULL );

    for( int32_t j = 0; j < MAX_SHADER_ATTRIBUTES; j++ ) {
        canvas->attributes[j].occupied = 0;
        canvas->buffer[j].occupied = 0;
    }

    for( int32_t i = 0; i < MAX_CANVAS_LAYERS; i++ ) {
        for( int32_t j = 0; j < MAX_CANVAS_TEXTURES+1; j++ ) {
            for( int32_t k = 0; k < MAX_CANVAS_SHADER; k++ ) {
                for( int32_t l = 0; l < MAX_CANVAS_PROJECTIONS; l++ ) {
                    for( int32_t m = 0; m < MAX_CANVAS_PRIMITIVES; m++ ) {
                        canvas->layer[i].indices[j][k][l][m].occupied = 0;
                    }
                }
            }
        }

        for( int32_t j = 0; j < MAX_CANVAS_FONTS; j++ ) {
            for( int32_t k = 0; k < MAX_CANVAS_PROJECTIONS; k++ ) {
                canvas->layer[i].text[j][k].occupied = 0;
            }
        }

        vec_copy4f((Vec4f){0, 0, 0, 1}, canvas->layer[i].cursor);
    }
}

void canvas_clear_shaders(struct Canvas* canvas) {
    for( int32_t i = 0; i < MAX_CANVAS_SHADER; i++ ) {
        canvas->shaders[i].name[0] = '\0';
    }
}

void canvas_clear_fonts(struct Canvas* canvas) {
    for( int32_t i = 0; i < MAX_CANVAS_FONTS; i++ ) {
        canvas->fonts[i].name[0] = '\0';
    }
}

void canvas_clear_textures(struct Canvas* canvas) {
    for( int32_t i = 0; i < MAX_CANVAS_TEXTURES; i++ ) {
        canvas->textures[i].name[0] = '\0';
    }
}


size_t canvas_append_attributes(struct Canvas* canvas, uint32_t attribute_i, uint32_t size, GLenum type, size_t n, void* attributes) {
    log_assert( canvas != NULL );

    if( canvas->components[attribute_i].size == 0 ) {
        static int warn_once = 1;
        if( warn_once ) {
            log_warn(__FILE__, __LINE__, "you tried to append attribute %d to a canvas without calling canvas_add_attribute first for that attribute\n", attribute_i);
            warn_once = 0;
        }
        return 0;
    }


    log_assert( size == canvas->components[attribute_i].size );
    log_assert( type == canvas->components[attribute_i].type );
    log_assert( ogl_sizeof_type(type) == canvas->components[attribute_i].bytes );

    if( n == 0 ) {
        return 0;
    }

    // this log_asserts makes sure that we do not create a situation where attributes and vertices are mismatched, it is
    // possible to add 3 vertices, and then just to add 2 texcoords, this would leave not only one vertex without a texcoord
    // but also the offsets of all following texcoords would be of by one, so we should only allow appending exactly the
    // amount of attributes that is needed to match the vertices that are already there
    if( attribute_i != SHADER_ATTRIBUTE_VERTEX ) {
        log_assert( n == canvas->attributes[SHADER_ATTRIBUTE_VERTEX].occupied - canvas->attributes[attribute_i].occupied );
    }

    size_t old_occupied = canvas->attributes[attribute_i].occupied;
    log_assert( INT32_MAX - n > old_occupied );
    size_t new_occupied = old_occupied + n;

    size_t alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->attributes[attribute_i].capacity ) {
        canvas_alloc_attributes(canvas, attribute_i, alloc);
        alloc = alloc * 2;
    }

    uint32_t attribute_size = canvas->components[attribute_i].size;
    uint32_t attribute_bytes = canvas->components[attribute_i].bytes;
    void* attribute_array = canvas->attributes[attribute_i].array;
    size_t n_bytes = n*attribute_size*attribute_bytes;
    memcpy((char*)attribute_array + old_occupied*attribute_size*attribute_bytes, (char*)attributes, n_bytes);

    canvas->attributes[attribute_i].occupied = new_occupied;

    return n;
}

size_t canvas_append_indices(struct Canvas* canvas, int32_t layer_i, int32_t texture_i, const char* shader_name, int32_t projection_i, GLenum primitive_type, size_t n, uint32_t* indices, size_t offset) {
    log_assert( canvas != NULL );
    log_assert( layer_i >= 0 );
    log_assert( n > 0 );
    log_assert( indices != NULL );
    log_assert( primitive_type == GL_TRIANGLES || primitive_type == GL_LINES );

    if( n == 0 ) {
        return 0;
    }

    int32_t shader_i = canvas_find_shader(canvas, shader_name);
    if( shader_i == MAX_CANVAS_SHADER ) {
        static int warn_once = 1;
        if( warn_once ) {
            log_warn(__FILE__, __LINE__, "shader \"%s\" not found\n", shader_name);
            log_warn(__FILE__, __LINE__, "using \"default_shader\" instead of \"%s\"\n", shader_name);
            warn_once = 0;
        }
        shader_i = canvas_find_shader(canvas, "default_shader");
    }

    if( shader_i == MAX_CANVAS_SHADER ) {
        log_fail(__FILE__, __LINE__, "no shader could be found in canvas when trying to append indices\n");
        return 0;
    }

    int32_t primitive_i = CANVAS_TRIANGLES;
    if( primitive_type == GL_LINES ) {
        primitive_i = CANVAS_LINES;
    }

    size_t old_occupied = canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].occupied;
    log_assert( INT32_MAX - n > old_occupied );
    size_t new_occupied = old_occupied + n;

    size_t alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].capacity ) {
        canvas_alloc_indices(canvas, layer_i, texture_i, shader_name, projection_i, primitive_type, alloc);
        alloc = alloc * 2;
    }

    if( offset > 0 ) {
        for( size_t i = 0; i < n; i++ ) {
            log_assert( offset + indices[i] < UINT_MAX );
            GLuint offset_index = (GLuint)offset + indices[i];
            canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].array[old_occupied+i] = offset_index;
        }
    } else {
        GLuint* indices_array = canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].array;
        size_t n_bytes = n*sizeof(uint32_t);
        memcpy((char*)indices_array + old_occupied*sizeof(GLuint), (char*)indices, n_bytes);
    }

    size_t vertices_capacity = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].capacity;
    size_t vertices_occupied = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].occupied;

    // the for loop below starts at one because we assume that 0 are the vertices, which we check
    // capcities against
    log_assert( SHADER_ATTRIBUTE_VERTEX == 0 );

    // we need to 'fill up' all attribute for which there are vertices but nothing added into the
    // buffers, so this is done here, this is neccessary so that we can have a canvas with e.g.
    // vertices/colors/texcoords but then only call append_vertices and append_texcoords, but still
    // advance the colors so that when we later _do_ call append_colors, the vertices and texcoords
    // and colors in the single, huge attribute buffers all line up correctly
    for( int32_t i = 1; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        size_t attribute_capacity = canvas->attributes[i].capacity;
        size_t attribute_occupied = canvas->attributes[i].occupied;
        uint32_t attribute_size = canvas->components[i].size;

        // an attribute is only filled when its components have a size, that means we called canvas_add_attribute
        // with a size > 0 before, adding the attribute
        if( attribute_size > 0 ) {

            // - we take the vertices occupied and compare them to the attribute occupied, if an attribute has
            // smaller occupied we'll just allocate new memory and zero it completely, then sync both occupied
            // and capacity counters
            // - the else case just handles when the capacity is already the same, enough, then we just sync the
            // occupied counters
            // - this originally compared capacities only, I later fixed it so it would compare occupied instead,
            // and also memset only the difference in occupied, instead of just everything
            if( attribute_occupied < vertices_occupied ) {
                uint32_t attribute_bytes = canvas->components[i].bytes;

                if( attribute_capacity < vertices_capacity ) {
                    canvas->attributes[i].array = realloc(canvas->attributes[i].array, vertices_capacity * attribute_size * attribute_bytes);
                    log_assert( canvas->attributes[i].array != NULL );
                }
                size_t dst_offset = attribute_occupied * attribute_size * attribute_bytes;
                size_t set_length = (vertices_occupied - attribute_occupied) * attribute_size * attribute_bytes;
                if( i == SHADER_ATTRIBUTE_VERTEX_COLOR ) {
                    memset((char*)canvas->attributes[i].array + dst_offset, 255, set_length);
                } else {
                    memset((char*)canvas->attributes[i].array + dst_offset, 0, set_length);
                }

                canvas->attributes[i].capacity = vertices_capacity;
                canvas->attributes[i].occupied = vertices_occupied;
            } else {
                canvas->attributes[i].occupied = vertices_occupied;
            }
        }
    }

    canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].occupied = new_occupied;

    return n;
}

size_t canvas_append_text(struct Canvas* canvas, int32_t layer_i, const char* font_name, int32_t projection_i, size_t n, uint32_t* indices, size_t offset) {
    log_assert( canvas != NULL );
    log_assert( layer_i >= 0 );
    log_assert( n > 0 );
    log_assert( indices != NULL );

    if( n == 0 ) {
        return 0;
    }

    int32_t font_i = canvas_find_font(canvas, font_name);
    if( font_i == MAX_CANVAS_FONTS ) {
        return 0;
    }

    size_t old_occupied = canvas->layer[layer_i].text[font_i][projection_i].occupied;
    size_t new_occupied = old_occupied + n;

    size_t alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->layer[layer_i].text[font_i][projection_i].capacity ) {
        canvas_alloc_text(canvas, layer_i, font_name, projection_i, alloc);
        alloc = alloc * 2;
    }

    if( offset > 0 ) {
        for( size_t i = 0; i < n; i++ ) {
            log_assert( offset + indices[i] < UINT32_MAX );
            uint32_t offset_index = (uint32_t)offset + indices[i];
            canvas->layer[layer_i].text[font_i][projection_i].array[old_occupied+i] = offset_index;
        }
    } else {
        GLuint* indices_array = canvas->layer[layer_i].text[font_i][projection_i].array;
        size_t n_bytes = n*sizeof(GLuint);
        memcpy((char*)indices_array + old_occupied*sizeof(GLuint), (char*)indices, n_bytes);
    }

    size_t vertices_capacity = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].capacity;
    size_t vertices_occupied = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].occupied;

    log_assert( SHADER_ATTRIBUTE_VERTEX == 0 );

    for( int32_t i = 1; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        size_t attribute_capacity = canvas->attributes[i].capacity;
        uint32_t attribute_size = canvas->components[i].size;

        if( attribute_size > 0 ) {
            if( attribute_capacity < vertices_capacity ) {
                uint32_t attribute_bytes = canvas->components[i].bytes;

                canvas->attributes[i].array = realloc(canvas->attributes[i].array, vertices_capacity * attribute_size * attribute_bytes);
                log_assert( canvas->attributes[i].array != NULL );
                memset((char*)canvas->attributes[i].array, 0, vertices_capacity * attribute_size * attribute_bytes);

                canvas->attributes[i].capacity = vertices_capacity;
                canvas->attributes[i].occupied = vertices_occupied;
            } else {
                canvas->attributes[i].occupied = vertices_occupied;
            }
        }
    }

    canvas->layer[layer_i].text[font_i][projection_i].occupied = new_occupied;

    return n;
}
