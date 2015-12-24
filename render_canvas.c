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

int canvas_render_create_mesh(struct Canvas* const canvas, int layer_i, const char* shader_name, GLenum primitive_type, struct Vbo* vbo, struct VboMesh* mesh) {
    assert( layer_i >= 0 );
    assert( layer_i <= NUM_CANVAS_LAYER );
    assert( canvas != NULL );

    int shader_i = canvas_find_shader(canvas, shader_name);
    if( shader_i == NUM_CANVAS_SHADER ) {
        return 0;
    }

    if( canvas->layer[layer_i].indices[shader_i][primitive_type].occupied <= 0 ||
        canvas->layer[layer_i].attributes[OGL_VERTICES].occupied <= 0 )
    {
        return 0;
    }

    int attributes_occupied = canvas->layer[layer_i].attributes[OGL_VERTICES].occupied;
    void* vertex_array = canvas->layer[layer_i].attributes[OGL_VERTICES].array;
    void* color_array = canvas->layer[layer_i].attributes[OGL_COLORS].array;

    int indices_occupied = canvas->layer[layer_i].indices[shader_i][primitive_type].occupied;
    void* indices_array = canvas->layer[layer_i].indices[shader_i][primitive_type].array;

    vbomesh_append_attributes(mesh, OGL_VERTICES, vertex_array, attributes_occupied);
    vbomesh_append_attributes(mesh, OGL_COLORS, color_array, attributes_occupied);
    // NOT FINISHED

    return vbomesh_append_indices(mesh, indices_array, indices_occupied);
}

int canvas_render_create_shader(struct Canvas* const canvas, const char* shader_name, struct Shader* shader) {
    assert( shader != NULL );

    int shader_i = canvas_find_shader(canvas, shader_name);
    if( shader_i == NUM_CANVAS_SHADER ) {
        return NUM_CANVAS_SHADER;
    }

    shader->vertex_shader = canvas->shader[shader_i].vertex_shader;
    shader->fragment_shader = canvas->shader[shader_i].fragment_shader;
    shader->program = canvas->shader[shader_i].program;

    for( int i = 0; i < NUM_OGL_ATTRIBUTES; i++ ) {
        strncpy(shader->attribute[i].name, canvas->shader[shader_i].attribute[i].name, strlen(canvas->shader[shader_i].attribute[i].name)+1);
        shader->attribute[i].location = canvas->shader[shader_i].attribute[i].location;
    }

    for( int i = 0; i < NUM_SHADER_UNIFORMS; i++ ) {
        strncpy(shader->uniform[i].name, "\0", 1);
        shader->uniform[i].location = -1;
    }

    static int canvas_uniform_map[NUM_SHADER_UNIFORMS*2] = {
        CANVAS_UNIFORM_MVP_MATRIX, SHADER_MVP_MATRIX,
        CANVAS_UNIFORM_NORMAL_MATRIX, SHADER_NORMAL_MATRIX,
        CANVAS_UNIFORM_AMBIENT_COLOR, SHADER_AMBIENT_COLOR,
        CANVAS_UNIFORM_DIFFUSE_COLOR, SHADER_DIFFUSE_COLOR
    };
    static const int num_canvas_uniforms = 4;

    for( int i = 0; i < num_canvas_uniforms*2; i += 2 ) {
        int src = canvas_uniform_map[i+0];
        int dst = canvas_uniform_map[i+1];
        if( canvas->shader[shader_i].uniform[src].location > -1 ) {
            strncpy(shader->uniform[dst].name, canvas->shader[shader_i].uniform[src].name, strlen(canvas->shader[shader_i].uniform[src].name)+1);
            shader->uniform[dst].location = canvas->shader[shader_i].uniform[src].location;
        }
    }

    return shader_i;
}

void canvas_render_layers(struct Canvas* const canvas, int layer_start, int layer_end, const char* shader_name, struct Camera* const camera, Mat const model_matrix, struct Shader* foo_shader) {
    if( layer_start == layer_end ) {
        layer_end += 1;
    }

    assert( layer_start >= 0 );
    assert( layer_end <= NUM_CANVAS_LAYER );
    assert( layer_start < layer_end );
    assert( canvas != NULL );

    int shader_i = canvas_find_shader(canvas, shader_name);
    if( shader_i == NUM_CANVAS_SHADER ) {
        return;
    }

    static int first_run = 1;

    static struct Shader shader[NUM_CANVAS_SHADER];
    if( first_run ) {
        for( int i = 0; i < NUM_CANVAS_SHADER; i++ ) {
            canvas_render_create_shader(canvas, shader_name, &shader[i]);
        }
    }

    static struct Vbo canvas_vbo;
    static struct VboMesh canvas_meshes[NUM_CANVAS_LAYER][NUM_OGL_PRIMITIVES];

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
                    if( canvas_render_create_mesh(canvas, layer_i, shader_name, primitive_j, &canvas_vbo, &canvas_meshes[layer_i][primitive_j]) ) {
                        vbomesh_render(&canvas_meshes[layer_i][primitive_j], &shader[shader_i], camera, model_matrix);
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
                    if( canvas_render_create_mesh(canvas, layer_i, shader_name, primitive_j, &canvas_vbo, &canvas_meshes[layer_i][primitive_j]) ) {
                        //vbomesh_print(&canvas_meshes[layer_i][primitive_j]);
                        vbomesh_render(&canvas_meshes[layer_i][primitive_j], &shader[shader_i], camera, model_matrix);
                    }
                }
            }
        }
    }
}
