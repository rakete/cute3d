#include "geometry_halfedgemesh.h"
#include "render_vbomesh.h"
#include "render_shader.h"
#include "cute_sdl2.h"
#include "cute_arcball.h"
#include "geometry_vbo.h"
#include "gui_draw.h"
#include "gui_canvas.h"
#include "render_canvas.h"

void vbomesh_from_solid(struct Solid* solid, struct VboMesh* mesh) {
    assert(solid->indices != NULL);
    assert(solid->vertices != NULL);
    assert(solid->normals != NULL);
    assert(solid->colors != NULL);

    vbomesh_append_attributes(mesh, OGL_VERTICES, solid->vertices, solid->size);
    vbomesh_append_attributes(mesh, OGL_NORMALS, solid->normals, solid->size);
    vbomesh_append_attributes(mesh, OGL_COLORS, solid->colors, solid->size);
    vbomesh_append_indices(mesh, solid->indices, solid->size);
}

void draw_grid2( struct Canvas* canvas,
                 int layer,
                 float width,
                 float height,
                 int steps,
                 const Color color,
                 const Mat model_matrix)
{
    int size = (steps+1)*2 + (steps+1)*2;

    float vertices[size * 3];
    float colors[size * 4];
    int elements[size];

    // 1  5  9 10----11
    // |  |  |
    // |  |  | 6-----7
    // |  |  |
    // 0  4  8 2-----3
    for( int i = 0; i < (steps+1); i++ ) {

        float xf = -width/2.0f + (float)i * (width / (float)steps);
        float yf = -height/2.0f + (float)i * (height / (float)steps);

        // a step includes one horizontal and one vertical line
        // made up of 2 vertices each, which makes 4 vertices in total
        // with 3 components which results in the number 12 below
        /* vertices[i * 12 + 0] = xf; */
        /* vertices[i * 12 + 1] = -height/2.0f; */
        /* vertices[i * 12 + 2] = 0.0; */
        mat_mul_vec3f(model_matrix, (Vec3f){xf, -height/2.0f, 0.0}, vertices + i*12 + 0);

        colors[i * 16 + 0] = color[0];
        colors[i * 16 + 1] = color[1];
        colors[i * 16 + 2] = color[2];
        colors[i * 16 + 3] = color[3];

        elements[i * 4 + 0] = i * 4 + 0;

        /* vertices[i * 12 + 3] = xf; */
        /* vertices[i * 12 + 4] = height/2.0f ; */
        /* vertices[i * 12 + 5] = 0.0f; */
        mat_mul_vec3f(model_matrix, (Vec3f){xf, height/2.0f, 0.0}, vertices + i*12 + 3);

        colors[i * 16 + 4] = color[0];
        colors[i * 16 + 5] = color[1];
        colors[i * 16 + 6] = color[2];
        colors[i * 16 + 7] = color[3];

        elements[i * 4 + 1] = i * 4 + 1;

        /* vertices[i * 12 + 6] = -width/2.0f; */
        /* vertices[i * 12 + 7] = yf; */
        /* vertices[i * 12 + 8] = 0.0; */
        mat_mul_vec3f(model_matrix, (Vec3f){-width/2.0f, yf, 0.0}, vertices + i*12 + 6);

        colors[i * 16 + 8] = color[0];
        colors[i * 16 + 9] = color[1];
        colors[i * 16 + 10] = color[2];
        colors[i * 16 + 11] = color[3];

        elements[i * 4 + 2] = i * 4 + 2;

        /* vertices[i * 12 + 9] = width/2.0f; */
        /* vertices[i * 12 + 10] = yf; */
        /* vertices[i * 12 + 11] = 0.0f; */
        mat_mul_vec3f(model_matrix, (Vec3f){width/2.0f, yf, 0.0}, vertices + i*12 + 9);

        colors[i * 16 + 12] = color[0];
        colors[i * 16 + 13] = color[1];
        colors[i * 16 + 14] = color[2];
        colors[i * 16 + 15] = color[3];

        elements[i * 4 + 3] = i * 4 + 3;
    }

    canvas_append_attributes(NULL, layer, vertices, colors, NULL, NULL, size);
    canvas_append_indices(NULL, layer, elements, size, GL_LINES);
}


int main(int argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-canvas", 0, 0, 800, 600, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( init_ogl(800, 600, (Color){0.0f, 0.0f, 0.0f, 1.0f}) ) {
        return 1;
    }

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas() ) {
        return 1;
    }

    struct Vbo vbo;
    vbo_create(&vbo);

    /* struct Shader shader; */
    /* shader_flat(&shader); */

    /* Vec light_direction = { 0.2, -0.5, -1.0 }; */
    /* shader_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction); */

    /* Color ambiance = { 0.25, 0.1, 0.2, 1.0 }; */
    /* shader_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance); */

    struct Shader shader;
    shader_lines_create(&shader);

    struct Arcball arcball;
    arcball_create(window, (Vec){1.0,2.0,6.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, 0.01, 1000.0, &arcball);

    Quat grid_rotation1;
    quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, (Vec){0.0, 1.0, 0.0, 1.0}, grid_rotation1);
    Mat grid_transform1;
    quat_to_mat(grid_rotation1, grid_transform1);

    Quat grid_rotation2;
    quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, (Vec){1.0, 0.0, 0.0, 1.0}, grid_rotation2);
    Mat grid_transform2;
    quat_to_mat(grid_rotation2, grid_transform2);

    struct Vbo canvas_vbo;
    vbo_create(&canvas_vbo);

    canvas_add_attributes(NULL, OGL_VERTICES, 3, GL_FLOAT, sizeof(float));
    canvas_add_attributes(NULL, OGL_COLORS, 4, GL_FLOAT, sizeof(float));

    struct VboMesh canvas_mesh;
    vbomesh_create(&canvas_vbo, GL_LINES, GL_UNSIGNED_INT, GL_DYNAMIC_DRAW, &canvas_mesh);

    while (true) {

        SDL_Event event;
        unsigned int counter = 0;
        while( SDL_PollEvent(&event) ) {
            switch (event.type) {
                case SDL_QUIT:
                    goto done;
                case SDL_KEYDOWN: {
                    SDL_KeyboardEvent* key_event = (SDL_KeyboardEvent*)&event;
                    if(key_event->keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        goto done;
                    }
                    break;
                }
            }

            arcball_event(&arcball, event);
        }

        sdl2_debug( SDL_GL_SetSwapInterval(1) );

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        draw_grid2(NULL, 0, 12.0f, 12.0f, 12, (Color){0.1, 0.8, 0.5, 1.0}, (Mat)IDENTITY_MAT);
        draw_grid2(NULL, 0, 12.0f, 12.0f, 12, (Color){0.1, 0.7, 0.9, 1.0}, grid_transform1);
        draw_grid2(NULL, 0, 12.0f, 12.0f, 12, (Color){0.9, 0.3, 0.4, 1.0}, grid_transform2);

        canvas_render_layers(NULL, 0, NUM_CANVAS_LAYER, &shader, &arcball.camera, (Mat)IDENTITY_MAT);

        canvas_clear(NULL, 0, NUM_CANVAS_LAYER);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }


done:
    return 0;
}
