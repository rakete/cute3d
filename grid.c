#include "grid.h"
#include "render.h"
#include "math_types.h"

#include "allegro5/allegro.h"

#include "GL/glut.h"


// x:3 y:3 z:3 = 27 cubes
// 
//  0:x0y0z0,  1:x1y0z0,  2:x2y0z0,  3:x0y1z0,  4:x1y1z0,  5:x2y1z0,  6:x0y2z0,  7:x1y2z0,  8:x2y2z0,
//  9:x0y0z1, 10:x1y0z1, 11:x2y0z1, 12:x0y1z1, 13:x1y1z1, 14:x2y1z1, 15:x0y2z1, 16:x1y2z1, 17:x2y2z1,
// 18:x0y0z2, 19:x1y0z2, 20:x2y0z2, 21:x0y1z2, 22:x1y1z2, 23:x2y1z2, 24:x0y2z2, 25:x1y2z2, 26:x2y2z2
//
//
//
//
//    
//   18.19 19.20 20.21
//  /   / /   / /   /
// 0...1 1...2 2...3
// |   | |   | |   |
// 4...5 5...6 6...7
// 4...5 5...6 6...7
// |   | |   | |   |
// 8...9 9..10 10.11
// 8...9 9..10 10.11
// |   | |   | |   |
// 12.13 14.15 16.17

// x:1 y:0 z:0
// 1...2
// |   |
// 5...6

//    0 1 2
// 0: 1 2 3
// 1: 4 5 6
// 2: 7 8 9
//
// 1 2 3 4 5 6 7 8 9
// 0 1 2 3 4 5 6 7 8
//
// box.dim = 2,2
// box.pos = 1,1
// grid.size = 3,3
//
// (1+0) + (1+0) * 3 = 4
// (1+1) + (1+0) * 3 = 5
// (1+0) + (1+1) * 3 = 7
// (1+1) + (1+1) * 3 = 8

uint64_t grid_index(struct Grid* grid, struct GridBox* box, uint64_t x, uint64_t y, uint64_t z) {
    return (box->position.x + x) + (box->position.y + y) * grid->size.x + (box->position.z + z) * grid->size.x * grid->size.y;
}

struct GridSize grid_size(struct Grid* grid, struct GridBox* box) {
    uint64_t boxsize = box->dimension.width * box->dimension.height * box->dimension.depth;

    uint64_t x = grid->size.x;
    uint64_t y = grid->size.y;
    uint64_t z = grid->size.z;
    uint64_t cubesize = x * y * z;
    for( int i = grid->top; i > box->level; i-- ) {
        if( x > 1 ) x /= 2;
        if( y > 1 ) y /= 2;
        if( z > 1 ) z /= 2;
        cubesize = x * y * z;
    }

    return( (struct GridSize){ .box = boxsize, .cubes = cubesize } );
}

void grid_create(uint64_t x, uint64_t y, uint64_t z,
                 float w, float h, float d,
                 uint64_t view_range, uint64_t chunk_size,
                 struct Grid* grid, struct Chunk** chunks)
{
    if( grid ) {
        grid->size.x = x;
        grid->size.y = y;
        grid->size.z = z;
        grid->cube.width = w;
        grid->cube.height = h;
        grid->cube.depth = d;
    
        grid->view.range = view_range;
    
        grid->chunk.size = chunk_size;
        grid->chunk.num = 0;

        grid->geometry_shader = 0;

        grid->top = 0;
        while( x > 1 || y > 1 || z > 1 ) {
            x /= 2;
            y /= 2;
            z /= 2;
            grid->top++;
        }
    }
}

void grid_set(struct Grid* grid, struct GridBox* box) {
}

struct Cube* grid_cube(struct Grid* grid, struct GridBox* box, uint64_t index, uint64_t cubeindex) {
}

void grid_clear(struct Grid* grid, struct GridBox* box) {
}

void grid_upload(struct Grid* grid, struct Chunk* chunks) {
}

int init_allegro() {
    return al_init();
}

void allegro_display( ALLEGRO_DISPLAY** display, int width, int height ) {
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
    al_set_new_display_flags(ALLEGRO_OPENGL);
    (*display) = al_create_display(width, height);

    glViewport(0,0,width,height);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
}

void allegro_events( ALLEGRO_EVENT_QUEUE** queue, ALLEGRO_DISPLAY* display ) {
    al_install_keyboard();
    al_install_mouse();

    (*queue) = al_create_event_queue();
    al_register_event_source((*queue), al_get_keyboard_event_source());
    al_register_event_source((*queue), al_get_mouse_event_source());
    al_register_event_source((*queue), al_get_display_event_source(display));
}

void default_render(Vec camera_translation, struct Mesh* mesh) {
    init_shader();
    struct Shader default_shader;
    shader_create(&default_shader, "shader/flat.vertex", "shader/flat.fragment");
    shader_attribute(&default_shader, vertex_array, "vertex");
    shader_attribute(&default_shader, color_array, "color");
    shader_attribute(&default_shader, normal_array, "normal");

    struct Camera default_camera;
    camera_create(&default_camera, 800, 600);
    camera_projection(&default_camera, perspective);
    //camera_projection(&default_camera, orthographic_zoom);
    camera_frustum(&default_camera, -0.5f, 0.5f, -0.375f, 0.375f, 1.0f, 200.0f);

    //Vec translation = { -0.6, -3.5, -8.0 };
    vector_add3f(default_camera.pivot.position, camera_translation, default_camera.pivot.position);
    Vec origin = { 0.0, 0.0, 0.0, 1.0 };
    pivot_lookat(&default_camera.pivot, origin);

    Matrix mesh_transform;
    matrix_identity(mesh_transform);
    render_mesh(mesh, &default_shader, &default_camera, mesh_transform);
}

int main(int argc, char *argv[]) {
    if( ! init_allegro() ) {
        return 1;
    }

    printf("display\n");
    ALLEGRO_DISPLAY* display;
    allegro_display(&display,800,600);

    printf("events\n");
    ALLEGRO_EVENT_QUEUE* events;
    allegro_events(&events, display);

    if( ! init_geometry() ) {
        return 1;
    }

    printf("vbo\n");
    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, vertex_array, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, normal_array, 3, GL_FLOAT, GL_STATIC_DRAW);

    printf("grid\n");
    struct Grid grid;
    struct Chunk* chunks;
    grid_create(128, 128, 128, 1.0, 1.0, 1.0, 8, 16, &grid, &chunks);
    printf("grid.top: %lu\n", grid.top);

    struct GridBox box = {
        .level = grid.top - 1,
        .dimension = { .width = 2, .height = 1, .depth = 2 },
        .position = { .x = 0, .y = 1, .z = 0 },
    };
    printf("box.level: %lu\n", box.level);

    struct GridSize gridsize = grid_size(&grid, &box);
    printf("gridsize.box: %lu\n", gridsize.box);
    printf("gridsize.cubes: %lu\n", gridsize.cubes);

    for( uint64_t b = 0; b < gridsize.box; b++ ) {
        for( uint64_t c = 0; c < gridsize.cubes; c++ ) {
            grid_index(&grid, &box, b, c);
        }
    }

    return 0;
}
