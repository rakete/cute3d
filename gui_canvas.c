#include "gui_canvas.h"

struct Canvas global_dynamic_canvas = {0};
struct Canvas global_static_canvas = {0};

int32_t init_canvas() {
    canvas_create("global_dynamic_canvas", &global_dynamic_canvas);
    canvas_create("global_static_canvas", &global_static_canvas);

    return 0;
}

void canvas_create(const char* name, struct Canvas* canvas) {
    log_assert( canvas != NULL );
    log_assert( strlen(name) > 0 );
    log_assert( strlen(name) < 256 );

    canvas->name[0] = '\0';
    strncat(canvas->name, name, strlen(name));

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        canvas->components[i].size = 0;
        if( i == SHADER_ATTRIBUTE_COLORS ) {
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
        shader_create(&canvas->shader[i]);
    }

    for( int32_t i = 0; i < MAX_CANVAS_FONTS; i++ ) {
        font_create(NULL, 0, NULL, 0, 0, NULL, NULL, &canvas->fonts[i]);
    }

    for( int32_t i = 0; i < MAX_CANVAS_LAYERS; i++ ) {
        for( int32_t j = 0; j < MAX_CANVAS_SHADER; j++ ) {
            for( int32_t k = 0; k < MAX_CANVAS_PROJECTIONS; k++ ) {
                for( int32_t l = 0; l < MAX_OGL_PRIMITIVES; l++ ) {
                    canvas->layer[i].indices[j][k][l].array = NULL;
                    canvas->layer[i].indices[j][k][l].id = 0;
                    canvas->layer[i].indices[j][k][l].occupied = 0;
                    canvas->layer[i].indices[j][k][l].capacity = 0;
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

    canvas->line_z_scaling = 1.0f;

    // canvas_create
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_NORMALS, 3, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_TEXCOORDS, 2, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_NEXT_VERTEX, 3, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_PREV_VERTEX, 3, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_LINE_THICKNESS, 1, GL_FLOAT);
    canvas_add_attribute(canvas, SHADER_ATTRIBUTE_BARYCENTRIC_COORDINATE, 3, GL_FLOAT);

    struct Shader shader = {0};
    shader_create_from_files("shader/default_shader.vert", "shader/default_shader.frag", "default_shader", &shader);
    log_assert( canvas_add_shader(canvas, &shader) < MAX_CANVAS_SHADER );

    struct Character symbols[256] = {0};
    default_font_create(symbols);

    struct Font font = {0};
    font_create(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;+-*/=()[]{}", 256, symbols, 9, 3, global_default_font_palette, "default_font", &font);

    log_assert( canvas_add_font(canvas, &font) < MAX_CANVAS_FONTS );
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
        size_t vertices_capacity = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].capacity;
        size_t vertices_occupied = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;
        if( vertices_capacity > 0 ) {
            // because this should be rare, I'll warn about it
            log_warn(__FILE__, __LINE__, "vertices already allocated when adding attribute %d\n", added_attribute);
            canvas->attributes[added_attribute].array = calloc(vertices_capacity * size, bytes);
            canvas->attributes[added_attribute].capacity = vertices_capacity;
            canvas->attributes[added_attribute].occupied = vertices_occupied;
        }
    }
}

int32_t canvas_add_shader(struct Canvas* canvas, const struct Shader* shader) {
    log_assert( shader != NULL );

    int32_t shader_i = 0;
    while( shader_i < MAX_CANVAS_SHADER && strlen(canvas->shader[shader_i].name) != 0 ) {
        if( strncmp(canvas->shader[shader_i].name, shader->name, 256) == 0 ) {
            log_warn(__FILE__, __LINE__, "shader \"%s\" already added to canvas \"%s\"\n", shader->name, canvas->name);
            return shader_i;
        }

        shader_i += 1;
    }

    if( shader_i == MAX_CANVAS_SHADER ) {
        log_warn(__FILE__, __LINE__, "no more space available in canvas for adding shader \"%s\"\n", shader->name);
        return MAX_CANVAS_SHADER;
    }

    memcpy(&canvas->shader[shader_i], shader, sizeof(struct Shader));

    return shader_i;
}

int32_t canvas_find_shader(struct Canvas* canvas, const char* shader_name) {
    size_t name_length = strlen(shader_name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    static int32_t check_first = -1;
    if( check_first > -1 &&
        check_first < MAX_CANVAS_SHADER &&
        strncmp(canvas->shader[check_first].name, shader_name, 256) == 0 )
    {
        return check_first;
    }

    int32_t shader_i = 0;
    while( shader_i < MAX_CANVAS_SHADER && strncmp(canvas->shader[shader_i].name, shader_name, 256) != 0 ) {
        shader_i += 1;
    }

    check_first = shader_i;

    return shader_i;
}

int32_t canvas_add_font(struct Canvas* canvas, const struct Font* font) {
    log_assert( font != NULL );

    int32_t font_i = 0;
    while( font_i < MAX_CANVAS_FONTS && strlen(canvas->fonts[font_i].name) != 0 ) {
        if( strncmp(canvas->fonts[font_i].name, font->name, 256) == 0 ) {
            log_warn(__FILE__, __LINE__, "font \"%s\" already added to canvas \"%s\"\n", font->name, canvas->name);
            return font_i;
        }

        font_i += 1;
    }

    if( font_i == MAX_CANVAS_FONTS ) {
        return MAX_CANVAS_FONTS;
    }

    memcpy(&canvas->fonts[font_i], font, sizeof(struct Font));

    return 0;
}

int32_t canvas_find_font(struct Canvas* canvas, const char* font_name) {
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
        font_i += 1;
    }

    if( font_i == MAX_CANVAS_FONTS && check_first == 0 ) {
        log_warn(__FILE__, __LINE__, "font \"%s\" not found\n", font_name);
    }

    check_first = font_i;

    return font_i;
}

size_t canvas_alloc_attributes(struct Canvas* canvas, uint32_t attribute_i, size_t n) {
    log_assert( canvas != NULL );
    log_assert( attribute_i >= 0 && attribute_i < MAX_SHADER_ATTRIBUTES );

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

size_t canvas_alloc_indices(struct Canvas* canvas, int32_t layer_i, int32_t projection_i, const char* shader_name, GLenum primitive_type, size_t n) {
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

    size_t old_capacity = canvas->layer[layer_i].indices[shader_i][projection_i][primitive_type].capacity;
    log_assert( INT32_MAX - n > old_capacity );
    size_t new_capacity = old_capacity + n;

    GLuint* old_array_pointer = canvas->layer[layer_i].indices[shader_i][projection_i][primitive_type].array;
    GLuint* new_array_pointer = realloc(old_array_pointer, new_capacity * sizeof(GLuint));

    log_assert( new_array_pointer != NULL );
    canvas->layer[layer_i].indices[shader_i][projection_i][primitive_type].array = new_array_pointer;
    canvas->layer[layer_i].indices[shader_i][projection_i][primitive_type].capacity = new_capacity;

    return n;
}

size_t canvas_alloc_text(struct Canvas* canvas, int32_t layer_i, int32_t text_i, const char* font_name, size_t n) {
    log_assert( canvas != NULL );
    log_assert( layer_i >= 0 );

    if( n == 0 ) {
        return 0;
    }

    int32_t font_i = canvas_find_font(canvas, font_name);
    if( font_i == MAX_CANVAS_FONTS ) {
        return 0;
    }

    size_t old_capacity = canvas->layer[layer_i].text[font_i][text_i].capacity;
    log_assert( INT32_MAX - n > old_capacity );
    size_t new_capacity = old_capacity + n;

    GLuint* old_array_pointer = canvas->layer[layer_i].text[font_i][text_i].array;
    GLuint* new_array_pointer = realloc(old_array_pointer, new_capacity * sizeof(GLuint));

    log_assert( new_array_pointer != NULL );
    canvas->layer[layer_i].text[font_i][text_i].array = new_array_pointer;
    canvas->layer[layer_i].text[font_i][text_i].capacity = new_capacity;

    return n;
}

void canvas_clear(struct Canvas* canvas) {
    log_assert( canvas != NULL );

    for( int32_t j = 0; j < MAX_SHADER_ATTRIBUTES; j++ ) {
        canvas->attributes[j].occupied = 0;
        canvas->buffer[j].occupied = 0;
    }

    for( int32_t i = 0; i < MAX_CANVAS_LAYERS; i++ ) {
        for( int32_t j = 0; j < MAX_CANVAS_SHADER; j++ ) {
            for( int32_t k = 0; k < MAX_CANVAS_PROJECTIONS; k++ ) {
                for( int32_t l = 0; l < MAX_OGL_PRIMITIVES; l++ ) {
                    canvas->layer[i].indices[j][k][l].occupied = 0;
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
    if( attribute_i != SHADER_ATTRIBUTE_VERTICES ) {
        log_assert( n == canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied - canvas->attributes[attribute_i].occupied );
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

size_t canvas_append_indices(struct Canvas* canvas, int32_t layer_i, int32_t projection_i, const char* shader_name, GLenum primitive_type, size_t n, uint32_t* indices, size_t offset) {
    log_assert( canvas != NULL );
    log_assert( layer_i >= 0 );
    log_assert( n > 0 );
    log_assert( indices != NULL );

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

    size_t old_occupied = canvas->layer[layer_i].indices[shader_i][projection_i][primitive_type].occupied;
    log_assert( INT32_MAX - n > old_occupied );
    size_t new_occupied = old_occupied + n;

    size_t alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->layer[layer_i].indices[shader_i][projection_i][primitive_type].capacity ) {
        canvas_alloc_indices(canvas, layer_i, projection_i, shader_name, primitive_type, alloc);
        alloc = alloc * 2;
    }

    if( offset > 0 ) {
        for( size_t i = 0; i < n; i++ ) {
            log_assert( offset + indices[i] < UINT_MAX );
            GLuint offset_index = (GLuint)offset + indices[i];
            canvas->layer[layer_i].indices[shader_i][projection_i][primitive_type].array[old_occupied+i] = offset_index;
        }
    } else {
        GLuint* indices_array = canvas->layer[layer_i].indices[shader_i][projection_i][primitive_type].array;
        size_t n_bytes = n*sizeof(uint32_t);
        memcpy((char*)indices_array + old_occupied*sizeof(GLuint), (char*)indices, n_bytes);
    }

    size_t vertices_capacity = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].capacity;
    size_t vertices_occupied = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;

    // the for loop below starts at one because we assume that 0 are the vertices, which we check
    // capcities against
    log_assert( SHADER_ATTRIBUTE_VERTICES == 0 );

    // we need to 'fill up' all attribute for which there are vertices but nothing added into the
    // buffers, so this is done here, this is neccessary so that we can have a canvas with e.g.
    // vertices/colors/texcoords but then only call append_vertices and append_texcoords, but still
    // advance the colors so that when we later _do_ call append_colors, the vertices and texcoords
    // and colors in the single, huge attribute buffers all line up correctly
    for( int32_t i = 1; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        size_t attribute_capacity = canvas->attributes[i].capacity;
        uint32_t attribute_size = canvas->components[i].size;

        // an attribute is only filled when its components have a size, that means we called canvas_add_attribute
        // with a size > 0 before, adding the attribute
        if( attribute_size > 0 ) {

            // we take the vertices capacity and compare them to the attribute capacity, if an attribute has
            // smaller capacity we'll just allocate new memory and zero it completely, then sync both occupied
            // and capacity counters
            // the else case just handles when the capacity is already the same, enough, then we just sync the
            // occupied counters
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

    canvas->layer[layer_i].indices[shader_i][projection_i][primitive_type].occupied = new_occupied;

    return n;
}

size_t canvas_append_text(struct Canvas* canvas, int32_t layer_i, int32_t text_i, const char* font_name, size_t n, uint32_t* indices, size_t offset) {
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

    size_t old_occupied = canvas->layer[layer_i].text[font_i][text_i].occupied;
    size_t new_occupied = old_occupied + n;

    size_t alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->layer[layer_i].text[font_i][text_i].capacity ) {
        canvas_alloc_text(canvas, layer_i, text_i, font_name, alloc);
        alloc = alloc * 2;
    }

    if( offset > 0 ) {
        for( size_t i = 0; i < n; i++ ) {
            log_assert( offset + indices[i] < UINT32_MAX );
            uint32_t offset_index = (uint32_t)offset + indices[i];
            canvas->layer[layer_i].text[font_i][text_i].array[old_occupied+i] = offset_index;
        }
    } else {
        GLuint* indices_array = canvas->layer[layer_i].text[font_i][text_i].array;
        size_t n_bytes = n*sizeof(GLuint);
        memcpy((char*)indices_array + old_occupied*sizeof(GLuint), (char*)indices, n_bytes);
    }

    size_t vertices_capacity = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].capacity;
    size_t vertices_occupied = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;

    log_assert( SHADER_ATTRIBUTE_VERTICES == 0 );

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

    canvas->layer[layer_i].text[font_i][text_i].occupied = new_occupied;

    return n;
}
