#include "world.h"
#include "allegro.h"

int main(int argc, char *argv[]) {

    if( ! init_allegro() ) {
        return 1;
    }

    ALLEGRO_DISPLAY* display;
    allegro_display(800,600,&display);

    ALLEGRO_EVENT_QUEUE* events;
    allegro_events(display, &events);

    if( ! init_geometry() ) {
        return 1;
    }

    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, vertex_array, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, normal_array, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, color_array, 4, GL_FLOAT, GL_STATIC_DRAW);

    struct Mesh world_mesh;
    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &world_mesh);

    struct Grid grid;
    grid_create(1,1,1,&grid);

    struct GridPages pages;
    grid_pages(&grid,1,1,1,&pages);

    uint64_t pages_num = pages.num.x * pages.num.y * pages.num.z;
    printf("%lu\n", pages_num);
    for( uint64_t i = 0; i < pages_num; i++ ) {
        grid_alloc(&pages, i, 0);
    }

    struct Cube cube;
    solid_cube(&cube);
    solid_normals((struct Solid*)&cube);
    
    world_grid_create(&grid, &pages, 0, 1.0, 1.0, 1.0, &cube, &world_mesh);

    pages.array[0][0][0] = 1;

    world_grid_update(&grid, &pages, 0, 0, &cube, &world_mesh);

    return 0;
}
