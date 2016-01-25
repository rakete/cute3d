#include "gui_canvas.h"

struct Canvas global_canvas = {
    .components = {{0}},
    .attributes = {{0}},
    .buffer = {{0}},
    .shader = {{{0}}},
    .fonts = {{{0}}},
    .layer = {{{{{0}}}}}
};

int32_t init_canvas() {
    canvas_create(&global_canvas);
    return 0;
}

void canvas_create(struct Canvas* canvas) {
    assert( canvas != NULL );

    for( int32_t i = 0; i < NUM_SHADER_ATTRIBUTES; i++ ) {
        canvas->components[i].size = 0;
        canvas->components[i].type = GL_FLOAT;
        canvas->components[i].bytes = 0;

        canvas->attributes[i].array = NULL;
        canvas->attributes[i].occupied = 0;
        canvas->attributes[i].capacity = 0;

        canvas->buffer[i].id = 0;
        canvas->buffer[i].occupied = 0;
        canvas->buffer[i].capacity = 0;
        canvas->buffer[i].usage = GL_STREAM_READ;
    }

    for( int32_t i = 0; i < NUM_CANVAS_SHADER; i++ ) {
        shader_create_empty(&canvas->shader[i]);
    }

    for( int32_t i = 0; i < NUM_CANVAS_LAYERS; i++ ) {
        for( int32_t j = 0; j < NUM_CANVAS_SHADER; j++ ) {
            for( int32_t k = 0; k < NUM_OGL_PRIMITIVES; k++ ) {
                canvas->layer[i].indices[j][k].array = NULL;
                canvas->layer[i].indices[j][k].id = 0;
                canvas->layer[i].indices[j][k].occupied = 0;
                canvas->layer[i].indices[j][k].capacity = 0;
            }
        }

        for( int32_t j = 0; j < NUM_CANVAS_FONTS; j++ ) {
            for( int32_t k = 0; k < NUM_CANVAS_PROJECTIONS; k++ ) {
                canvas->layer[i].text[j][k].array = NULL;
                canvas->layer[i].text[j][k].id = 0;
                canvas->layer[i].text[j][k].occupied = 0;
                canvas->layer[i].text[j][k].capacity = 0;
            }
        }

        vec_copy((Vec4f){0, 0, 0, 1}, canvas->layer[i].cursor);
    }
}

void canvas_add_attribute(struct Canvas* canvas, int32_t added_attribute, uint32_t size, GLenum type) {
    assert( canvas != NULL );
    assert( added_attribute >= 0 && added_attribute < NUM_SHADER_ATTRIBUTES );
    assert( size > 0 );

    if( canvas->components[added_attribute].size <= 0 ) {

        canvas->components[added_attribute].size = size;
        canvas->components[added_attribute].type = type;
        uint32_t bytes = (uint32_t)ogl_sizeof_type(type);
        canvas->components[added_attribute].bytes = bytes;

        size_t capacity = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].capacity;
        if( capacity > 0 ) {
            canvas->attributes[added_attribute].array = calloc(capacity * size, bytes);
        }
    }
}

int32_t canvas_append_shader(struct Canvas* canvas, struct Shader* const shader, const char* shader_name) {
    size_t name_length = strlen(shader_name);
    assert( name_length > 0 );
    assert( name_length < 256 );

    int32_t shader_i = 0;
    while( shader_i < NUM_CANVAS_SHADER && strlen(canvas->shader[shader_i].name) != 0 ) {
        if( strncmp(canvas->shader[shader_i].name, shader_name, 256) == 0 ) {
            return shader_i;
        }

        shader_i += 1;
    }

    if( shader_i == NUM_CANVAS_SHADER ) {
        return NUM_CANVAS_SHADER;
    }

    shader_copy(shader, &canvas->shader[shader_i]);

    return shader_i;
}

int32_t canvas_find_shader(struct Canvas* canvas, const char* shader_name) {
    size_t name_length = strlen(shader_name);
    assert( name_length > 0 );
    assert( name_length < 256 );

    static int32_t check_first = 0;
    if( check_first > 0 &&
        check_first < NUM_CANVAS_SHADER &&
        strncmp(canvas->shader[check_first].name, shader_name, 256) == 0 )
    {
        return check_first;
    }

    int32_t shader_i = 0;
    while( shader_i < NUM_CANVAS_SHADER && strncmp(canvas->shader[shader_i].name, shader_name, 256) != 0 ) {
        shader_i += 1;
    }

    if( shader_i == NUM_CANVAS_SHADER && check_first == 0 ) {
        log_warn(stderr, __FILE__, __LINE__, "shader \"%s\" not found\n", shader_name);
    }

    check_first = shader_i;

    return shader_i;
}

int32_t canvas_append_font(struct Canvas* canvas, struct Font font, const char* font_name) {
    size_t name_length = strlen(font_name);
    assert( name_length > 0 );
    assert( name_length < 256 );

    int32_t font_i = 0;
    while( font_i < NUM_CANVAS_FONTS && strlen(canvas->fonts[font_i].name) != 0 ) {
        if( strncmp(canvas->fonts[font_i].name, font_name, 256) == 0 ) {
            return font_i;
        }

        font_i += 1;
    }

    if( font_i == NUM_CANVAS_FONTS ) {
        return NUM_CANVAS_FONTS;
    }

    memcpy(&canvas->fonts[font_i], &font, sizeof(struct Font));

    return 0;
}

int32_t canvas_find_font(struct Canvas* canvas, const char* font_name) {
    size_t name_length = strlen(font_name);
    assert( name_length > 0 );
    assert( name_length < 256 );

    static int32_t check_first = 0;
    if( check_first > 0 &&
        check_first < NUM_CANVAS_FONTS &&
        strncmp(canvas->fonts[check_first].name, font_name, 256) == 0 )
    {
        return check_first;
    }

    int32_t font_i = 0;
    while( font_i < NUM_CANVAS_FONTS && strncmp(canvas->fonts[font_i].name, font_name, 256) != 0 ) {
        font_i += 1;
    }

    if( font_i == NUM_CANVAS_FONTS && check_first == 0 ) {
        log_warn(stderr, __FILE__, __LINE__, "font \"%s\" not found\n", font_name);
    }

    check_first = font_i;

    return font_i;
}

size_t canvas_alloc_attributes(struct Canvas* canvas, int32_t attribute_i, size_t n) {
    assert( canvas != NULL );
    assert( attribute_i >= 0 && attribute_i < NUM_SHADER_ATTRIBUTES );

    if( n == 0 ) {
        return 0;
    }

    size_t old_capacity = canvas->attributes[attribute_i].capacity;
    assert( INT32_MAX - n > old_capacity );
    size_t new_capacity = old_capacity + n;

    uint32_t size = canvas->components[attribute_i].size;

    if( size > 0 ) {
        float* old_array_pointer = canvas->attributes[attribute_i].array;
        float* new_array_pointer = (float*)realloc(old_array_pointer, new_capacity * size * sizeof(float));

        if( new_array_pointer != NULL ) {
            canvas->attributes[attribute_i].array = new_array_pointer;
        }

        assert( canvas->attributes[attribute_i].array != NULL );
    } else {
        assert( canvas->attributes[attribute_i].array == NULL );
    }

    canvas->attributes[attribute_i].capacity = new_capacity;

    return n;
}

size_t canvas_alloc_indices(struct Canvas* canvas, int32_t layer_i, const char* shader_name, GLenum primitive_type, size_t n) {
    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( primitive_type == GL_LINES || primitive_type == GL_TRIANGLES || primitive_type == GL_QUADS );

    if( n == 0 ) {
        return 0;
    }

    int32_t shader_i = canvas_find_shader(canvas, shader_name);
    if( shader_i == NUM_CANVAS_SHADER ) {
        return 0;
    }

    size_t old_capacity = canvas->layer[layer_i].indices[shader_i][primitive_type].capacity;
    assert( INT32_MAX - n > old_capacity );
    size_t new_capacity = old_capacity + n;

    GLuint* old_array_pointer = canvas->layer[layer_i].indices[shader_i][primitive_type].array;
    GLuint* new_array_pointer = realloc(old_array_pointer, new_capacity * sizeof(GLuint));

    if( new_array_pointer != NULL ) {
        canvas->layer[layer_i].indices[shader_i][primitive_type].array = new_array_pointer;
        canvas->layer[layer_i].indices[shader_i][primitive_type].capacity = new_capacity;
        return n;
    }

    return 0;
}

size_t canvas_alloc_text(struct Canvas* canvas, int32_t layer_i, int32_t text_i, const char* font_name, size_t n) {
    assert( canvas != NULL );
    assert( layer_i >= 0 );

    if( n == 0 ) {
        return 0;
    }

    int32_t font_i = canvas_find_font(canvas, font_name);
    if( font_i == NUM_CANVAS_FONTS ) {
        return 0;
    }

    size_t old_capacity = canvas->layer[layer_i].text[font_i][text_i].capacity;
    assert( INT32_MAX - n > old_capacity );
    size_t new_capacity = old_capacity + n;

    GLuint* old_array_pointer = canvas->layer[layer_i].text[font_i][text_i].array;
    GLuint* new_array_pointer = realloc(old_array_pointer, new_capacity * sizeof(GLuint));

    if( new_array_pointer != NULL ) {
        canvas->layer[layer_i].text[font_i][text_i].array = new_array_pointer;
        canvas->layer[layer_i].text[font_i][text_i].capacity = new_capacity;
        return n;
    }

    return 0;
}

void canvas_clear(struct Canvas* canvas, int32_t layer_start, int32_t layer_end) {
    assert( layer_start >= 0 );
    assert( layer_end <= NUM_CANVAS_LAYERS );
    assert( layer_start < layer_end );
    assert( canvas != NULL );

    for( int32_t j = 0; j < NUM_SHADER_ATTRIBUTES; j++ ) {
        canvas->attributes[j].occupied = 0;
        canvas->buffer[j].occupied = 0;
    }

    for( int32_t i = layer_start; i < layer_end; i++ ) {
        for( int32_t j = 0; j < NUM_CANVAS_SHADER; j++ ) {
            for( int32_t k = 0; k < NUM_OGL_PRIMITIVES; k++ ) {
                canvas->layer[i].indices[j][k].occupied = 0;
            }
        }

        for( int32_t j = 0; j < NUM_CANVAS_FONTS; j++ ) {
            for( int32_t k = 0; k < NUM_CANVAS_PROJECTIONS; k++ ) {
                canvas->layer[i].text[j][k].occupied = 0;
            }
        }

        vec_copy((Vec4f){0, 0, 0, 1}, canvas->layer[i].cursor);
    }

}

size_t canvas_append_vertices(struct Canvas* canvas, void* vertices, uint32_t size, GLenum type, size_t n, const Mat model_matrix) {
    assert( canvas != NULL );
    assert( size == canvas->components[SHADER_ATTRIBUTE_VERTICES].size );
    assert( type == canvas->components[SHADER_ATTRIBUTE_VERTICES].type );
    assert( sizeof(type) == canvas->components[SHADER_ATTRIBUTE_VERTICES].bytes );

    if( n == 0 ) {
        return 0;
    }

    size_t old_occupied = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;
    assert( INT32_MAX - n > old_occupied );
    size_t new_occupied = old_occupied + n;

    size_t alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->attributes[SHADER_ATTRIBUTE_VERTICES].capacity ) {
        canvas_alloc_attributes(canvas, SHADER_ATTRIBUTE_VERTICES, alloc);
        alloc = alloc * 2;
    }

    uint32_t vertex_size = canvas->components[SHADER_ATTRIBUTE_VERTICES].size;
    uint32_t vertex_bytes = canvas->components[SHADER_ATTRIBUTE_VERTICES].bytes;
    void* vertex_array = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].array;

    assert( vertex_size <= 4 );
    assert( vertex_bytes <= 8 );

    size_t n_bytes = n*vertex_size*vertex_bytes;
    if( vertices == NULL && vertex_size > 0 ) {
        memset((char*)vertex_array + old_occupied*vertex_size*vertex_bytes, 0, n_bytes);
    } else if( vertices != NULL && model_matrix == NULL ) {
        memcpy((char*)vertex_array + old_occupied*vertex_size*vertex_bytes, (char*)vertices, n_bytes);
    } else if( vertices != NULL && model_matrix != NULL ) {
        for( size_t i = 0; i < n; i++ ) {
            float* src = (float*)((char*)vertices + i*vertex_size*vertex_bytes);
            float* dst = (float*)((char*)vertex_array + (old_occupied+i)*vertex_size*vertex_bytes);
            mat_mul_vec3f(model_matrix, src, dst);
        }
    }

    canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied = new_occupied;

    return n;
}

size_t canvas_append_colors(struct Canvas* canvas, void* colors, uint32_t size, GLenum type, size_t n, const Color color) {
    assert( canvas != NULL );
    assert( size == canvas->components[SHADER_ATTRIBUTE_COLORS].size );
    assert( type == canvas->components[SHADER_ATTRIBUTE_COLORS].type );
    assert( sizeof(type) == canvas->components[SHADER_ATTRIBUTE_COLORS].bytes );

    if( n == 0 ) {
        return 0;
    }

    size_t old_occupied = canvas->attributes[SHADER_ATTRIBUTE_COLORS].occupied;
    assert( INT32_MAX - n > old_occupied );
    size_t new_occupied = old_occupied + n;

    size_t alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->attributes[SHADER_ATTRIBUTE_COLORS].capacity ) {
        canvas_alloc_attributes(canvas, SHADER_ATTRIBUTE_COLORS, alloc);
        alloc = alloc * 2;
    }

    uint32_t color_size = canvas->components[SHADER_ATTRIBUTE_COLORS].size;
    uint32_t color_bytes = canvas->components[SHADER_ATTRIBUTE_COLORS].bytes;
    void* color_array = canvas->attributes[SHADER_ATTRIBUTE_COLORS].array;

    assert( color_size <= 4 );
    assert( color_bytes <= 8 );

    size_t n_bytes = n*color_size*color_bytes;
    if( colors == NULL && color_size > 0 && color != NULL) {
        for( size_t i = 0; i < n; i++ ) {
            memcpy((char*)color_array + (old_occupied+i)*color_size*color_bytes, (char*)color, color_size*color_bytes);
        }
    } else if( colors == NULL && color_size > 0 && color == NULL) {
        memset((char*)color_array + old_occupied*color_size*color_bytes, 0, n_bytes);
    } if( colors != NULL ) {
        memcpy((char*)color_array + old_occupied*color_size*color_bytes, (char*)colors, n_bytes);
    }

    canvas->attributes[SHADER_ATTRIBUTE_COLORS].occupied = new_occupied;

    return n;
}

size_t canvas_append_texcoords(struct Canvas* canvas, void* texcoords, uint32_t size, GLenum type, size_t n) {
    assert( canvas != NULL );
    assert( size == canvas->components[SHADER_ATTRIBUTE_TEXCOORDS].size );
    assert( type == canvas->components[SHADER_ATTRIBUTE_TEXCOORDS].type );
    assert( sizeof(type) == canvas->components[SHADER_ATTRIBUTE_TEXCOORDS].bytes );

    if( n == 0 ) {
        return 0;
    }

    size_t old_occupied = canvas->attributes[SHADER_ATTRIBUTE_TEXCOORDS].occupied;
    assert( INT32_MAX - n > old_occupied );
    size_t new_occupied = old_occupied + n;

    size_t alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->attributes[SHADER_ATTRIBUTE_TEXCOORDS].capacity ) {
        canvas_alloc_attributes(canvas, SHADER_ATTRIBUTE_TEXCOORDS, alloc);
        alloc = alloc * 2;
    }

    uint32_t texcoord_size = canvas->components[SHADER_ATTRIBUTE_TEXCOORDS].size;
    uint32_t texcoord_bytes = canvas->components[SHADER_ATTRIBUTE_TEXCOORDS].bytes;
    void* texcoord_array = canvas->attributes[SHADER_ATTRIBUTE_TEXCOORDS].array;

    assert( texcoord_size <= 4 );
    assert( texcoord_bytes <= 8 );

    size_t n_bytes = n*texcoord_size*texcoord_bytes;
    if( texcoords == NULL && texcoord_size > 0 ) {
        memset((char*)texcoord_array + old_occupied*texcoord_size*texcoord_bytes, 0, n_bytes);
    } if( texcoords != NULL ) {
        memcpy((char*)texcoord_array + old_occupied*texcoord_size*texcoord_bytes, (char*)texcoords, n_bytes);
    }

    canvas->attributes[SHADER_ATTRIBUTE_TEXCOORDS].occupied = new_occupied;

    return n;
}

size_t canvas_append_indices(struct Canvas* canvas, int32_t layer_i, const char* shader_name, GLenum primitive_type, uint32_t* indices, size_t n, size_t offset) {
    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( n > 0 );
    assert( indices != NULL );

    if( n == 0 ) {
        return 0;
    }

    int32_t shader_i = canvas_find_shader(canvas, shader_name);
    if( shader_i == NUM_CANVAS_SHADER ) {
        return 0;
    }

    size_t old_occupied = canvas->layer[layer_i].indices[shader_i][primitive_type].occupied;
    assert( INT32_MAX - n > old_occupied );
    size_t new_occupied = old_occupied + n;

    size_t alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->layer[layer_i].indices[shader_i][primitive_type].capacity ) {
        canvas_alloc_indices(canvas, layer_i, shader_name, primitive_type, alloc);
        alloc = alloc * 2;
    }

    if( offset > 0 ) {
        for( size_t i = 0; i < n; i++ ) {
            assert( offset + indices[i] < UINT_MAX );
            GLuint offset_index = (GLuint)offset + indices[i];
            canvas->layer[layer_i].indices[shader_i][primitive_type].array[old_occupied+i] = offset_index;
        }
    } else {
        GLuint* indices_array = canvas->layer[layer_i].indices[shader_i][primitive_type].array;
        size_t n_bytes = n*sizeof(uint32_t);
        memcpy((char*)indices_array + old_occupied*sizeof(GLuint), (char*)indices, n_bytes);
    }

    size_t vertices_capacity = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].capacity;
    for( int32_t i = 1; i < NUM_SHADER_ATTRIBUTES; i++ ) {
        size_t attribute_capacity = canvas->attributes[i].capacity;
        if( attribute_capacity < vertices_capacity ) {
            uint32_t attribute_size = canvas->components[i].size;
            uint32_t attribute_bytes = canvas->components[i].bytes;
            canvas->attributes[i].array = calloc((vertices_capacity - attribute_capacity) * attribute_size, attribute_bytes);
        }
    }

    canvas->layer[layer_i].indices[shader_i][primitive_type].occupied = new_occupied;

    return n;
}

size_t canvas_append_text(struct Canvas* canvas, int32_t layer_i, int32_t text_i, const char* font_name, uint32_t* indices, size_t n, size_t offset) {
    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( n > 0 );
    assert( indices != NULL );

    if( n == 0 ) {
        return 0;
    }

    int32_t font_i = canvas_find_font(canvas, font_name);
    if( font_i == NUM_CANVAS_FONTS ) {
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
            assert( offset + indices[i] < UINT32_MAX );
            uint32_t offset_index = (uint32_t)offset + indices[i];
            canvas->layer[layer_i].text[font_i][text_i].array[old_occupied+i] = offset_index;
        }
    } else {
        GLuint* indices_array = canvas->layer[layer_i].text[font_i][text_i].array;
        size_t n_bytes = n*sizeof(GLuint);
        memcpy((char*)indices_array + old_occupied*sizeof(GLuint), (char*)indices, n_bytes);
    }

    size_t vertices_capacity = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].capacity;
    for( int32_t i = 1; i < NUM_SHADER_ATTRIBUTES; i++ ) {
        size_t attribute_capacity = canvas->attributes[i].capacity;
        if( attribute_capacity < vertices_capacity ) {
            uint32_t attribute_size = canvas->components[i].size;
            uint32_t attribute_bytes = canvas->components[i].bytes;
            canvas->attributes[i].array = calloc((vertices_capacity - attribute_capacity) * attribute_size, attribute_bytes);
        }
    }

    canvas->layer[layer_i].text[font_i][text_i].occupied = new_occupied;

    return n;
}
