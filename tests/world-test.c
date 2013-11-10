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
    vbo_add_buffer(&vbo, color_array, 4, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, normal_array, 3, GL_FLOAT, GL_STATIC_DRAW);

    struct Mesh world_mesh;
    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &world_mesh);

    struct Grid grid;
    grid_create(4,4,4,&grid);

    struct GridPages pages;
    grid_pages(&grid,2,2,2,&pages);

    uint64_t pages_num = pages.num.x * pages.num.y * pages.num.z;
    printf("%lu\n", pages_num);
    for( uint64_t i = 0; i < pages_num; i++ ) {
        grid_alloc(&pages, i, 0);
    }

    return 0;
}
