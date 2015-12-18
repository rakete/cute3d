#include "render_canvas.h"

/* void canvas_render_create(struct Canvas* const canvas, struct Vbo* vbo, struct CanvasRender* render) { */
/*     assert( vbo != NULL ); */
/*     render->canvas = canvas; */

/*     vbo_add_buffer(vbo, OGL_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW); */
/*     vbo_add_buffer(vbo, OGL_COLORS, 4, GL_FLOAT, GL_STATIC_DRAW); */
/*     vbo_add_buffer(vbo, OGL_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW); */
/*     vbo_add_buffer(vbo, OGL_TEXCOORDS, 2, GL_FLOAT, GL_STATIC_DRAW); */
/*     render->vbo = vbo; */

/*     for( int i = 0; i < NUM_CANVAS_SHADER; i++ ) { */
/*         render->shader[i] = NULL; */
/*     } */
/* } */

/* void canvas_render_shader(struct CanvasRender* render, int i, struct Shader* shader) { */
/*     render->shader[i] = shader; */
/* } */

int canvas_render_create(struct Canvas* const canvas_pointer, int layer_i, GLenum primitive_type, struct Vbo* vbo, struct VboMesh* mesh) {
    assert( layer_i >= 0 );
    assert( layer_i <= NUM_CANVAS_LAYER );

    struct Canvas* canvas = canvas_pointer;
    if( canvas_pointer == NULL ) {
        canvas = &global_canvas;
    }

    assert( canvas != NULL );

    if( canvas->layer[layer_i].indices[primitive_type].occupied <= 0 || canvas->layer[layer_i].attributes.occupied <= 0 ) {
        return 0;
    }

    int attributes_occupied = canvas->layer[layer_i].attributes.occupied;
    void* vertex_array = canvas->layer[layer_i].attributes.array[OGL_VERTICES];
    void* color_array = canvas->layer[layer_i].attributes.array[OGL_COLORS];
    int indices_occupied = canvas->layer[layer_i].indices[primitive_type].occupied;
    void* indices_array = canvas->layer[layer_i].indices[primitive_type].array;

    vbomesh_append_attributes(mesh, OGL_VERTICES, vertex_array, attributes_occupied);
    vbomesh_append_attributes(mesh, OGL_COLORS, color_array, attributes_occupied);
    return vbomesh_append_indices(mesh, indices_array, indices_occupied);
}

void canvas_render_layers(struct Canvas* const canvas_pointer, int layer_start, int layer_end, struct Shader* const shader, struct Camera* const camera, Mat const model_matrix) {
    if( layer_start == layer_end ) {
        layer_end += 1;
    }

    assert( layer_start >= 0 );
    assert( layer_end <= NUM_CANVAS_LAYER );
    assert( layer_start < layer_end );

    struct Canvas* canvas = canvas_pointer;
    if( canvas_pointer == NULL ) {
        canvas = &global_canvas;
    }

    assert( canvas != NULL );

    static struct Vbo canvas_vbo;
    static struct VboMesh canvas_meshes[NUM_CANVAS_LAYER][NUM_OGL_PRIMITIVES];
    static int first_run = 1;

    if( first_run ) {
        vbo_create(&canvas_vbo);

        for( int i = 0; i < NUM_OGL_ATTRIBUTES; i++ ) {
            if( canvas->components[i].size > 0 ) {
                vbo_add_buffer(&canvas_vbo, i, canvas->components[i].size, canvas->components[i].type, GL_DYNAMIC_DRAW);
            }
        }

        for( int layer_i = 0; layer_i < NUM_CANVAS_LAYER; layer_i++ ) {
            for( int primitive_j = 0; primitive_j < NUM_OGL_PRIMITIVES; primitive_j++ ) {
                vbomesh_create(&canvas_vbo, primitive_j, GL_UNSIGNED_INT, GL_DYNAMIC_DRAW, &canvas_meshes[layer_i][primitive_j]);

                if( layer_i >= layer_start && layer_i < layer_end ) {
                    if( canvas_render_create(canvas, layer_i, primitive_j, &canvas_vbo, &canvas_meshes[layer_i][primitive_j]) ) {
                        vbomesh_render(&canvas_meshes[layer_i][primitive_j], shader, camera, model_matrix);
                    }
                }
            }
        }

        first_run = 0;
    } else {
        for( int layer_i = 0; layer_i < NUM_CANVAS_LAYER; layer_i++ ) {
            for( int primitive_j = 0; primitive_j < NUM_OGL_PRIMITIVES; primitive_j++ ) {
                vbomesh_clear_attributes(&canvas_meshes[layer_i][primitive_j]);
                vbomesh_clear_indices(&canvas_meshes[layer_i][primitive_j]);

                if( layer_i >= layer_start && layer_i < layer_end ) {
                    if( canvas_render_create(canvas, layer_i, primitive_j, &canvas_vbo, &canvas_meshes[layer_i][primitive_j]) ) {
                        //vbomesh_print(&canvas_meshes[layer_i][primitive_j]);
                        vbomesh_render(&canvas_meshes[layer_i][primitive_j], shader, camera, model_matrix);
                    }
                }
            }
        }
    }
}
