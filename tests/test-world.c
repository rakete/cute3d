/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#include "driver_vbo.h"
#include "driver_sdl2.h"

#include "geometry_solid.h"
#include "geometry_grid.h"

int32_t main(int32_t argc, char *argv[]) {

    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-world", 100, 60, 1280, 720, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, &context);

    if( init_vbo() ) {
        return 1;
    }

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    struct Ibo ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &ibo);

    struct VboMesh world_mesh = {0};
    vbo_mesh_create(&vbo, &ibo, &world_mesh);

    struct Grid grid = {0};
    grid_create(1, 1, 1, &grid);

    struct GridPages pages = {0};
    grid_pages(&grid, 1, 1, 1, &pages);

    uint64_t pages_num = pages.num.x * pages.num.y * pages.num.z;
    printf("%lu\n", pages_num);
    for( uint64_t i = 0; i < pages_num; i++ ) {
        grid_alloc(&pages, i, 0);
    }

    /* struct SolidBox cube = {0}; */
    /* solid_cube_create(1.0, (Color){255, 0, 0, 255}, &cube); */

    /* world_grid_create(&pages, 0, 1.0, 1.0, 1.0, &cube, &world_mesh); */

    /* pages.array[0][0][0] = 1; */

    /* world_grid_update(&grid, &pages, 0, 0, &cube, &world_mesh); */

    return 0;
}
