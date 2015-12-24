#include "gui_canvas.h"

struct Canvas global_canvas = {
    .layer = {},
};

int init_canvas() {
    canvas_create(&global_canvas);
    return 0;
}

void canvas_create(struct Canvas* canvas) {
    assert( canvas != NULL );

    for( int i = 0; i < NUM_OGL_ATTRIBUTES; i++ ) {
        canvas->components[i].size = 0;
        canvas->components[i].type = GL_FLOAT;
        canvas->components[i].bytes = 0;
    }

    //strncpy(canvas->shader[0].name, "default", 8);

    for( int i = 0; i < NUM_CANVAS_SHADER; i++ ) {
        strncpy(canvas->shader[i].name, "\0", 1);
        canvas->shader[i].program = 0;

        for( int j = 0; j < NUM_OGL_ATTRIBUTES; j++ ) {
            canvas->shader[i].attribute[j].location = -1;
            strncpy(canvas->shader[i].attribute[j].name, "\0", 1);
        }

        for( int j = 0; j < NUM_CANVAS_UNIFORMS; j++ ) {
            canvas->shader[i].uniform[j].location = -1;
            strncpy(canvas->shader[i].uniform[j].name, "\0", 1);
        }
    }

    for( int i = 0; i < NUM_CANVAS_LAYER; i++ ) {
        for( int j = 0; j < NUM_OGL_ATTRIBUTES; j++ ) {
            canvas->layer[i].attributes[j].array = NULL;
            canvas->layer[i].attributes[j].occupied = 0;
            canvas->layer[i].attributes[j].capacity = 0;
        }

        for( int j = 0; j < NUM_CANVAS_SHADER; j++ ) {
            for( int k = 0; k < NUM_OGL_PRIMITIVES; k++ ) {
                canvas->layer[i].indices[j][k].array = NULL;
                canvas->layer[i].indices[j][k].occupied = 0;
                canvas->layer[i].indices[j][k].capacity = 0;
            }
        }

        canvas->layer[i].screen = 0.0f;
    }
}

void canvas_add_attribute(struct Canvas* canvas, int added_attribute, int size, GLenum type, int bytes) {
    assert( canvas != NULL );
    assert( added_attribute >= 0 && added_attribute < NUM_OGL_ATTRIBUTES );
    assert( size > 0 );

    if( canvas->components[added_attribute].size <= 0 ) {
        canvas->components[added_attribute].size = size;
        canvas->components[added_attribute].type = type;
        canvas->components[added_attribute].bytes = bytes;

        for( int i = 0; i < NUM_CANVAS_LAYER; i++ ) {
            int capacity = canvas->layer[i].attributes[OGL_VERTICES].capacity;
            if( capacity > 0 ) {
                canvas->layer[i].attributes[added_attribute].array = calloc(capacity * size, bytes);
            }
        }
    }
}

int canvas_append_shader_source(struct Canvas* canvas, const char* vertex_source, const char* fragment_source, const char* name) {
    return -1;
}

int canvas_append_shader_program(struct Canvas* canvas, GLuint vertex_shader, GLuint fragment_shader, GLuint program, const char* name) {
    int name_length = strlen(name);
    assert( name_length > 0 );
    assert( name_length < 256 );

    int shader_i = 0;
    while( shader_i < NUM_CANVAS_SHADER && strlen(canvas->shader[shader_i].name) != 0 ) {
        if( strcmp(canvas->shader[shader_i].name, name) == 0 ) {
            return shader_i;
        }

        shader_i += 1;
    }

    if( shader_i == NUM_CANVAS_SHADER ) {
        return NUM_CANVAS_SHADER;
    }

    strncpy(canvas->shader[shader_i].name, name, strlen(name)+1);
    canvas->shader[shader_i].vertex_shader = vertex_shader;
    canvas->shader[shader_i].fragment_shader = fragment_shader;
    canvas->shader[shader_i].program = program;

    const char attribute_names[4][256] = {"vertex", "color", "texcoord", "normal"};
    const int attribute_types[4] = {OGL_VERTICES, OGL_COLORS, OGL_TEXCOORDS, OGL_NORMALS};
    const int attribute_checks = 4;
    for( int j = 0; j < attribute_checks; j++ ) {
        const char* name = attribute_names[j];
        const int type = attribute_types[j];

        int location = glGetAttribLocation(program, name);
        if( location > -1 ) {
            canvas->shader[shader_i].attribute[type].location = location;
            strncpy(canvas->shader[shader_i].attribute[type].name, name, strlen(name)+1);
        }
    }

    const char uniform_names[4][256] = {"mvp_matrix", "normal_matrix", "ambient_color", "diffuse_color"};
    const int uniform_types[4] = {CANVAS_UNIFORM_MVP_MATRIX, CANVAS_UNIFORM_NORMAL_MATRIX, CANVAS_UNIFORM_AMBIENT_COLOR, CANVAS_UNIFORM_DIFFUSE_COLOR};
    const int uniform_checks = 4;
    for( int j = 0; j < uniform_checks; j++ ) {
        const char* name = uniform_names[j];
        const int type = uniform_types[j];

        int location = glGetUniformLocation(program, name);
        if( location > -1 ) {
            canvas->shader[shader_i].uniform[type].location = location;
            strncpy(canvas->shader[shader_i].uniform[type].name, name, strlen(name)+1);
        }
    }

    return shader_i;
}

int canvas_find_shader(struct Canvas* canvas, const char* shader_name) {
    int name_length = strlen(shader_name);
    assert( name_length > 0 );
    assert( name_length < 256 );

    static int check_first = 0;
    if( check_first > 0 &&
        check_first < NUM_CANVAS_SHADER &&
        strcmp(canvas->shader[check_first].name, shader_name) == 0 )
    {
        return check_first;
    }

    int shader_i = 0;
    while( shader_i < NUM_CANVAS_SHADER && strcmp(canvas->shader[shader_i].name, shader_name) != 0 ) {
        shader_i += 1;
    }

    if( shader_i < NUM_CANVAS_SHADER ) {
        check_first = shader_i;
    }

    return shader_i;
}

int canvas_alloc_attributes(struct Canvas* canvas, int layer_i, int attribute_i, int n) {
    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( attribute_i >= 0 && attribute_i < NUM_OGL_ATTRIBUTES );
    assert( n >= 0 );

    if( n == 0 ) {
        return 0;
    }

    int old_capacity = canvas->layer[layer_i].attributes[attribute_i].capacity;
    int new_capacity = old_capacity + n;

    int size = canvas->components[attribute_i].size;

    if( size > 0 ) {
        float* old_array_pointer = canvas->layer[layer_i].attributes[attribute_i].array;
        float* new_array_pointer = (float*)realloc(old_array_pointer, new_capacity * size * sizeof(float));

        if( new_array_pointer != NULL ) {
            canvas->layer[layer_i].attributes[attribute_i].array = new_array_pointer;
        }

        assert( canvas->layer[layer_i].attributes[attribute_i].array != NULL );
    } else {
        assert( canvas->layer[layer_i].attributes[attribute_i].array == NULL );
    }

    canvas->layer[layer_i].attributes[attribute_i].capacity = new_capacity;

    return n;
}

int canvas_alloc_indices(struct Canvas* canvas, int layer_i, const char* shader_name, GLenum primitive_type, int n) {
    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( n >= 0 );
    assert( primitive_type == GL_LINES || primitive_type == GL_TRIANGLES || primitive_type == GL_QUADS );

    if( n == 0 ) {
        return 0;
    }

    int shader_i = canvas_find_shader(canvas, shader_name);
    if( shader_i == NUM_CANVAS_SHADER ) {
        return 0;
    }

    int old_capacity = canvas->layer[layer_i].indices[shader_i][primitive_type].capacity;
    int new_capacity = old_capacity + n;

    unsigned int* old_array_pointer = canvas->layer[layer_i].indices[shader_i][primitive_type].array;
    unsigned int* new_array_pointer = realloc(old_array_pointer, new_capacity * sizeof(int));

    if( new_array_pointer != NULL ) {
        canvas->layer[layer_i].indices[shader_i][primitive_type].array = new_array_pointer;
        canvas->layer[layer_i].indices[shader_i][primitive_type].capacity = new_capacity;
        return n;
    }

    return 0;
}

void canvas_clear(struct Canvas* canvas, int layer_start, int layer_end) {
    assert( layer_start >= 0 );
    assert( layer_end <= NUM_CANVAS_LAYER );
    assert( layer_start < layer_end );
    assert( canvas != NULL );

    for( int i = layer_start; i < layer_end; i++ ) {
        for( int j = 0; j < NUM_OGL_ATTRIBUTES; j++ ) {
            canvas->layer[i].attributes[j].occupied = 0;
        }

        for( int j = 0; j < NUM_CANVAS_SHADER; j++ ) {
            for( int k = 0; k < NUM_OGL_PRIMITIVES; k++ ) {
                canvas->layer[i].indices[j][k].occupied = 0;
            }
        }
    }
}

int canvas_append_vertices(struct Canvas* canvas, int layer_i, void* vertices, int n, const Mat model_matrix) {
    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( n >= 0 );
    assert( vertices != NULL );

    if( n == 0 ) {
        return 0;
    }

    int old_occupied = canvas->layer[layer_i].attributes[OGL_VERTICES].occupied;
    int new_occupied = old_occupied + n;

    int alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->layer[layer_i].attributes[OGL_VERTICES].capacity ) {
        canvas_alloc_attributes(canvas, layer_i, OGL_VERTICES, alloc);
        alloc = alloc * 2;
    }

    assert( canvas->components[OGL_VERTICES].size > 0 );

    int vertex_size = canvas->components[OGL_VERTICES].size;
    int vertex_bytes = canvas->components[OGL_VERTICES].bytes;
    void* vertex_array = canvas->layer[layer_i].attributes[OGL_VERTICES].array;

    assert( vertex_size <= 4 );
    assert( vertex_bytes <= 8 );

    if( vertices == NULL && vertex_size > 0 ) {
        memset((char*)vertex_array + old_occupied*vertex_size*vertex_bytes, 0, n*vertex_size*vertex_bytes);
    } else if( vertices != NULL && model_matrix == NULL ) {
        memcpy((char*)vertex_array + old_occupied*vertex_size*vertex_bytes, (char*)vertices, n*vertex_size*vertex_bytes);
    } else if( vertices != NULL && model_matrix != NULL ) {
        for( int i = 0; i < n; i++ ) {
            float* src = (float*)((char*)vertices + i*vertex_size*vertex_bytes);
            float* dst = (float*)((char*)vertex_array + (old_occupied+i)*vertex_size*vertex_bytes);
            mat_mul_vec3f(model_matrix, src, dst);
        }
    }

    canvas->layer[layer_i].attributes[OGL_VERTICES].occupied = new_occupied;

    return n;
}

int canvas_append_colors(struct Canvas* canvas, int layer_i, void* colors, int n, const Color color) {
    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( n >= 0 );

    if( n == 0 ) {
        return 0;
    }

    int old_occupied = canvas->layer[layer_i].attributes[OGL_COLORS].occupied;
    int new_occupied = old_occupied + n;

    int alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->layer[layer_i].attributes[OGL_COLORS].capacity ) {
        canvas_alloc_attributes(canvas, layer_i, OGL_COLORS, alloc);
        alloc = alloc * 2;
    }

    assert( canvas->components[OGL_COLORS].size > 0 );

    int color_size = canvas->components[OGL_COLORS].size;
    int color_bytes = canvas->components[OGL_COLORS].bytes;
    void* color_array = canvas->layer[layer_i].attributes[OGL_COLORS].array;

    assert( color_size <= 4 );
    assert( color_bytes <= 8 );

    if( colors == NULL && color_size > 0 && color != NULL) {
        for( int i = 0; i < n; i++ ) {
            memcpy((char*)color_array + (old_occupied+i)*color_size*color_bytes, (char*)color, color_size*color_bytes);
        }
    } else if( colors == NULL && color_size > 0 && color == NULL) {
        memset((char*)color_array + old_occupied*color_size*color_bytes, 0, n*color_size*color_bytes);
    } if( colors != NULL ) {
        memcpy((char*)color_array + old_occupied*color_size*color_bytes, (char*)colors, n*color_size*color_bytes);
    }

    canvas->layer[layer_i].attributes[OGL_COLORS].occupied = new_occupied;

    return n;
}

int canvas_append_indices(struct Canvas* canvas, int layer_i, const char* shader_name, GLenum primitive_type, unsigned int* indices, int n, int offset) {
    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( n > 0 );
    assert( indices != NULL );

    if( n == 0 ) {
        return 0;
    }

    int shader_i = canvas_find_shader(canvas, shader_name);
    if( shader_i == NUM_CANVAS_SHADER ) {
        return 0;
    }

    int old_occupied = canvas->layer[layer_i].indices[shader_i][primitive_type].occupied;
    int new_occupied = old_occupied + n;

    int alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->layer[layer_i].indices[shader_i][primitive_type].capacity ) {
        canvas_alloc_indices(canvas, layer_i, shader_name, primitive_type, alloc);
        alloc = alloc * 2;
    }

    if( offset > 0 ) {
        for( int i = 0; i < n; i++ ) {
            canvas->layer[layer_i].indices[shader_i][primitive_type].array[old_occupied+i] = offset + indices[i];
        }
    } else {
        unsigned int* indices_array = canvas->layer[layer_i].indices[shader_i][primitive_type].array;
        memcpy((char*)indices_array + old_occupied*sizeof(unsigned int), (char*)indices, n*sizeof(unsigned int));
    }

    int vertices_capacity = canvas->layer[layer_i].attributes[OGL_VERTICES].capacity;
    for( int i = 1; i < NUM_OGL_ATTRIBUTES; i++ ) {
        int attribute_capacity = canvas->layer[layer_i].attributes[i].capacity;
        if( attribute_capacity < vertices_capacity ) {
            int attribute_size = canvas->components[i].size;
            int attribute_bytes = canvas->components[i].bytes;
            canvas->layer[i].attributes[i].array = calloc((vertices_capacity - attribute_capacity) * attribute_size, attribute_bytes);
        }
    }

    canvas->layer[layer_i].indices[shader_i][primitive_type].occupied = new_occupied;

    return n;
}
