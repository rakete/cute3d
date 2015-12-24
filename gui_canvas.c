#include "gui_canvas.h"

struct Canvas global_canvas = {
    .layer = {},
};

int init_canvas() {
    canvas_create(&global_canvas);
    return 0;
}

void canvas_create(struct Canvas* canvas) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    assert( canvas != NULL );

    for( int i = 0; i < NUM_OGL_ATTRIBUTES; i++ ) {
        canvas->components[i].size = 0;
        canvas->components[i].type = GL_FLOAT;
        canvas->components[i].bytes = 0;
    }

    for( int i = 0; i < NUM_CANVAS_LAYER; i++ ) {
        for( int j = 0; j < NUM_OGL_ATTRIBUTES; j++ ) {
            canvas->layer[i].attributes.array[j] = NULL;
        }

        canvas->layer[i].attributes.occupied = 0;
        canvas->layer[i].attributes.capacity = 0;

        for( int j = 0; j < NUM_OGL_PRIMITIVES; j++ ) {
            canvas->layer[i].indices[j].array = NULL;
            canvas->layer[i].indices[j].occupied = 0;
            canvas->layer[i].indices[j].capacity = 0;
        }

        canvas->layer[i].screen = 0.0f;
    }
}

void canvas_add_attributes(struct Canvas* canvas, int attribute_i, int size, GLenum type, int bytes) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    assert( canvas != NULL );
    assert( attribute_i >= 0 && attribute_i < NUM_OGL_ATTRIBUTES );
    assert( size > 0 );

    if( canvas->components[attribute_i].size <= 0 ) {
        canvas->components[attribute_i].size = size;
        canvas->components[attribute_i].type = type;
        canvas->components[attribute_i].bytes = bytes;

        for( int layer_i = 0; layer_i < NUM_CANVAS_LAYER; layer_i++ ) {
            int capacity = canvas->layer[layer_i].attributes.capacity;
            if( capacity > 0 ) {
                canvas->layer[layer_i].attributes.array[attribute_i] = calloc(capacity * size, bytes);
            }
        }
    }
}

int canvas_alloc_attributes(struct Canvas* canvas, int layer_i, int n) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( n >= 0 );

    if( n == 0 ) {
        return 0;
    }

    int old_capacity = canvas->layer[layer_i].attributes.capacity;
    int new_capacity = old_capacity + n;

    for( int attribute_i = 0; attribute_i < NUM_OGL_ATTRIBUTES; attribute_i++ ) {
        int size = canvas->components[attribute_i].size;

        if( size > 0 ) {
            float* old_array_pointer = canvas->layer[layer_i].attributes.array[attribute_i];
            float* new_array_pointer = (float*)realloc(old_array_pointer, new_capacity * size * sizeof(float));

            if( new_array_pointer != NULL ) {
                canvas->layer[layer_i].attributes.array[attribute_i] = new_array_pointer;
            }

            assert( canvas->layer[layer_i].attributes.array[attribute_i] != NULL );
        } else {
            assert( canvas->layer[layer_i].attributes.array[attribute_i] == NULL );
        }
    }

    canvas->layer[layer_i].attributes.capacity = new_capacity;

    return n;
}

int canvas_alloc_indices(struct Canvas* canvas, int layer_i, int n, GLenum primitive_type) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( n >= 0 );
    assert( primitive_type == GL_LINES || primitive_type == GL_TRIANGLES || primitive_type == GL_QUADS );

    if( n == 0 ) {
        return 0;
    }

    int old_capacity = canvas->layer[layer_i].indices[primitive_type].capacity;
    int new_capacity = old_capacity + n;

    int* old_array_pointer = canvas->layer[layer_i].indices[primitive_type].array;
    int* new_array_pointer = realloc(old_array_pointer, new_capacity * sizeof(int));

    if( new_array_pointer != NULL ) {
        canvas->layer[layer_i].indices[primitive_type].array = new_array_pointer;
        canvas->layer[layer_i].indices[primitive_type].capacity = new_capacity;
        return n;
    }

    return 0;
}

void canvas_clear(struct Canvas* canvas, int layer_start, int layer_end) {
    assert( layer_start >= 0 );
    assert( layer_end <= NUM_CANVAS_LAYER );
    assert( layer_start < layer_end );

    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    assert( canvas != NULL );

    for( int i = layer_start; i < layer_end; i++ ) {
        for( int j = 0; j < NUM_OGL_ATTRIBUTES; j++ ) {
            canvas->layer[i].attributes.occupied = 0;
        }

        for( int j = 0; j < NUM_OGL_PRIMITIVES; j++ ) {
            canvas->layer[i].indices[j].occupied = 0;
        }
    }
}

void canvas_append_attributes(struct Canvas* canvas, int layer_i, void* vertices, void* colors, void* normals, void* texcoords, int n) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( n >= 0 );
    assert( vertices != NULL || normals != NULL || texcoords != NULL || colors != NULL );

    if( n == 0 ) return;

    int old_occupied = canvas->layer[layer_i].attributes.occupied;
    int new_occupied = old_occupied + n;

    int alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->layer[layer_i].attributes.capacity ) {
        canvas_alloc_attributes(canvas, layer_i, alloc);
        alloc = alloc * 2;
    }

    if( vertices ) {
        assert( canvas->components[OGL_VERTICES].size > 0 );
    }

    if( colors ) {
        assert( canvas->components[OGL_COLORS].size > 0 );
    }

    if( normals ) {
        assert( canvas->components[OGL_NORMALS].size > 0 );
    }

    if( texcoords ) {
        assert( canvas->components[OGL_TEXCOORDS].size > 0 );
    }

    int vertex_size = canvas->components[OGL_VERTICES].size;
    int vertex_bytes = canvas->components[OGL_VERTICES].bytes;
    void* vertex_array = canvas->layer[layer_i].attributes.array[OGL_VERTICES];

    int color_size = canvas->components[OGL_COLORS].size;
    int color_bytes = canvas->components[OGL_COLORS].bytes;
    void* color_array = canvas->layer[layer_i].attributes.array[OGL_COLORS];

    int normal_size = canvas->components[OGL_NORMALS].size;
    int normal_bytes = canvas->components[OGL_NORMALS].bytes;
    void* normal_array = canvas->layer[layer_i].attributes.array[OGL_NORMALS];

    int texcoord_size = canvas->components[OGL_TEXCOORDS].size;
    int texcoord_bytes = canvas->components[OGL_TEXCOORDS].bytes;
    void* texcoord_array = canvas->layer[layer_i].attributes.array[OGL_TEXCOORDS];

    assert( vertex_size <= 4 );
    assert( vertex_bytes <= 8 );
    assert( color_size <= 4 );
    assert( color_bytes <= 8 );
    assert( normal_size <= 4 );
    assert( normal_bytes <= 8 );
    assert( texcoord_size <= 4 );
    assert( texcoord_bytes <= 4 );

    if( vertex_size > 0 && vertices == NULL ) {
        memset((char*)vertex_array + old_occupied*vertex_size*vertex_bytes, 0, n*vertex_size*vertex_bytes);
    } else {
        memcpy((char*)vertex_array + old_occupied*vertex_size*vertex_bytes, (char*)vertices, n*vertex_size*vertex_bytes);
    }

    if( color_size > 0 && colors == NULL ) {
        memset((char*)color_array + old_occupied*color_size*color_bytes, 0, n*color_size*color_bytes);
    } else {
        memcpy((char*)color_array + old_occupied*color_size*color_bytes, (char*)colors, n*color_size*color_bytes);
    }

    if( normal_size > 0 && normals == NULL ) {
        memset((char*)normal_array + old_occupied*normal_size*normal_bytes, 0, n*normal_size*normal_bytes);
    } else {
        memcpy((char*)normal_array + old_occupied*normal_size*normal_bytes, (char*)normals, n*normal_size*normal_bytes);
    }

    if( texcoord_size > 0 && texcoords == NULL ) {
        memset((char*)texcoord_array + old_occupied*texcoord_size*texcoord_bytes, 0, n*texcoord_size*texcoord_bytes);
    } else {
        memcpy((char*)texcoord_array + old_occupied*texcoord_size*texcoord_bytes, (char*)texcoords, n*texcoord_size*normal_bytes);
    }

    canvas->layer[layer_i].attributes.occupied = new_occupied;
}

void canvas_append_indices(struct Canvas* canvas, int layer_i, int* indices, int n, GLenum primitive_type) {
    if( canvas == NULL ) {
        canvas = &global_canvas;
    }

    assert( canvas != NULL );
    assert( layer_i >= 0 );
    assert( n > 0 );
    assert( indices != NULL );

    int old_occupied = canvas->layer[layer_i].indices[primitive_type].occupied;
    int new_occupied = old_occupied + n;

    int alloc = DEFAULT_CANVAS_ALLOC;
    while( new_occupied > canvas->layer[layer_i].indices[primitive_type].capacity ) {
        canvas_alloc_indices(canvas, layer_i, alloc, primitive_type);
        alloc = alloc * 2;
    }

    int offset = canvas->layer[layer_i].indices[primitive_type].occupied;
    for( int i = 0; i < n; i++ ) {
        canvas->layer[layer_i].indices[primitive_type].array[old_occupied+i] = offset + indices[i];
    }

    canvas->layer[layer_i].indices[primitive_type].occupied = new_occupied;
}
