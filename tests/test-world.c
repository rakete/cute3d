#include "world.h"
#include "sdl2.h"

int main(int argc, char *argv[]) {

    if( ! init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-world", 0, 0, 800, 600, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( ! init_geometry() ) {
        return 1;
    }

    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, VERTEX_ARRAY, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, NORMAL_ARRAY, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, COLOR_ARRAY, 4, GL_FLOAT, GL_STATIC_DRAW);

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
    solid_cube(1.0, &cube);
    solid_normals((struct Solid*)&cube);

    world_grid_create(&grid, &pages, 0, 1.0, 1.0, 1.0, &cube, &world_mesh);

    pages.array[0][0][0] = 1;

    world_grid_update(&grid, &pages, 0, 0, &cube, &world_mesh);

    return 0;
}
