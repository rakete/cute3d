#include "geometry_grid.h"

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

struct GridBox* grid_pagebox_xyz(struct Grid* grid, struct GridPages* pages, uint64_t x, uint64_t y, uint64_t z, uint32_t level, struct GridBox* box) {
    if( grid && pages && box ) {
        box->position.x = x * pages->size.x;
        box->position.y = y * pages->size.y;
        box->position.z = z * pages->size.z;

        box->size.x = pages->size.x;
        box->size.y = pages->size.y;
        box->size.z = pages->size.z;

        box->level = level;
    }

    return box;
}

struct GridBox* grid_pagebox(struct Grid* grid, struct GridPages* pages, uint64_t page, uint32_t level, struct GridBox* box) {
    uint64_t z = page / (pages->num.x * pages->num.y);
    uint64_t y = page % (pages->num.x * pages->num.y) / pages->num.x;
    uint64_t x = page % (pages->num.x * pages->num.y) % pages->num.x;

    return grid_pagebox_xyz(grid, pages, x, y, z, level, box);
}

struct GridBox* grid_levelup(struct Grid* grid, struct GridPages* pages, struct GridBox* box) {
    if( box && box->level < pages->top ) {
        box->level++;

        box->size.x /= 2;
        box->size.y /= 2;
        box->size.z /= 2;

        box->position.x /= 2;
        box->position.y /= 2;
        box->position.z /= 2;
    }
    return box;
}

struct GridBox* grid_leveldown(struct Grid* grid, struct GridPages* pages, struct GridBox* box) {
    if( box && box->level > 0 ) {
        box->level--;

        box->size.x *= 2;
        box->size.y *= 2;
        box->size.z *= 2;

        box->position.x *= 2;
        box->position.y *= 2;
        box->position.z *= 2;
    }
    return box;
}

struct GridIndex* grid_index_xyz(struct Grid* grid, struct GridPages* pages, struct GridBox* box, uint64_t x, uint64_t y, uint64_t z, struct GridIndex* index) {
    if( NULL == box && grid ) {
        box = &box_createN(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    uint32_t level = box->level;

    if( x >= box->size.x ) x = box->size.x - 1;
    if( y >= box->size.y ) y = box->size.y - 1;
    if( z >= box->size.z ) z = box->size.z - 1;

    if( pages && pages->size.x > 0 && pages->size.y > 0 && pages->size.z > 0 && index ) {
        struct GridSize size;
        grid_pagesize(pages, box->level, &size);
        uint64_t pagesize_x = size.x;
        uint64_t pagesize_y = size.y;
        uint64_t pagesize_z = size.z;

        uint64_t page_x = (box->position.x + x) / pagesize_x;
        uint64_t page_y = (box->position.y + y) / pagesize_y;
        uint64_t page_z = (box->position.z + z) / pagesize_z;

        uint64_t cell_x = (box->position.x + x) % pagesize_x;
        uint64_t cell_y = (box->position.y + y) % pagesize_y;
        uint64_t cell_z = (box->position.z + z) % pagesize_z;

        uint64_t page = page_z * pages->num.x * pages->num.y + page_y * pages->num.x + page_x;
        uint64_t cell = cell_z * pagesize_x * pagesize_y + cell_y * pagesize_x + cell_x;

        index->page = page;
        index->level = level;
        index->cell = cell;
        return index;
    } else {
        return NULL;
    }
}

struct GridIndex* grid_index(struct Grid* grid, struct GridPages* pages, struct GridBox* box, uint64_t i, struct GridIndex* index) {
    if( NULL == box ) {
        box = &box_createN(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    uint64_t z = i / (box->size.x * box->size.y);
    uint64_t y = i % (box->size.x * box->size.y) / box->size.x;
    uint64_t x = i % (box->size.x * box->size.y) % box->size.x;
    //printf("%lu | %lu %lu %lu | %lu %lu %lu\n", i, box->size.x, box->size.y, box->size.z, x, y, z);

    return grid_index_xyz(grid,pages,box,x,y,z,index);
}

struct GridSize* grid_levelsize(struct Grid* grid, struct GridPages* pages, uint64_t level, struct GridSize* size) {
    if( grid && pages && size ) {
        if( level > pages->top ) {
            level = pages->top;
        }

        size->x = grid->size.x > 1 ? grid->size.x / ((uint64_t)1 << level) : 1;
        size->y = grid->size.y > 1 ? grid->size.y / ((uint64_t)1 << level) : 1;
        size->z = grid->size.z > 1 ? grid->size.z / ((uint64_t)1 << level) : 1;
        size->array = size->x * size->y * size->z;
    }

    return size;
}

struct GridSize* grid_pagesize(struct GridPages* pages, uint64_t level, struct GridSize* size) {
    if( pages && size ) {
        if( level > pages->top ) {
            level = pages->top;
        }

        size->x = pages->size.x > 1 ? pages->size.x / ((uint64_t)1 << level) : 1;
        size->y = pages->size.y > 1 ? pages->size.y / ((uint64_t)1 << level) : 1;
        size->z = pages->size.z > 1 ? pages->size.z / ((uint64_t)1 << level) : 1;
        size->array = size->x * size->y * size->z;
    }

    return size;
}

void grid_create(uint64_t x, uint64_t y, uint64_t z,
                 struct Grid* grid)
{
    if( grid ) {
        grid->size.x = x;
        grid->size.y = y;
        grid->size.z = z;
    }
}

void grid_pages(struct Grid* grid, uint64_t x, uint64_t y, uint64_t z, struct GridPages* pages) {
    if( pages &&
        grid->size.x % x == 0 &&
        grid->size.y % y == 0 &&
        grid->size.z % z == 0 )
    {
        pages->num.x = grid->size.x / x;
        pages->num.y = grid->size.y / y;
        pages->num.z = grid->size.z / z;

        pages->size.x = x;
        pages->size.y = y;
        pages->size.z = z;

        pages->top = 0;
        struct GridSize size;
        while( grid_pagesize(pages, pages->top, &size)->array > 1 ) {
            pages->top++;
        }

        pages->array = (Page*)calloc(x * y * z, sizeof(Page*));
        for( int32_t i = 0; i < x * y * z; i++ ) {
            pages->array[i] = (Cell**)calloc(pages->top, sizeof(Cell**));
        }
    }
}

void grid_dump(struct Grid grid, struct GridPages pages) {
    printf("grid.size.x: %" PRIu64 "\n", grid.size.x);
    printf("grid.size.y: %" PRIu64 "\n", grid.size.y);
    printf("grid.size.z: %" PRIu64 "\n", grid.size.z);
    printf("pages.size.x: %" PRIu64 "\n", pages.size.x);
    printf("pages.size.y: %" PRIu64 "\n", pages.size.y);
    printf("pages.size.z: %" PRIu64 "\n", pages.size.z);
    printf("pages.top: %" PRIu64 "\n", pages.top);

    if( pages.size.x > 0 && pages.size.y > 0 && pages.size.z > 0 ) {
        uint64_t num_pages_x = grid.size.x / pages.size.x;
        uint64_t num_pages_y = grid.size.y / pages.size.y;
        uint64_t num_pages_z = grid.size.z / pages.size.z;
        printf("num_pages_x: %" PRIu64 "\n", num_pages_x);
        printf("num_pages_y: %" PRIu64 "\n", num_pages_y);
        printf("num_pages_z: %" PRIu64 "\n", num_pages_z);
    }

    if( pages.top > 0 ) {
        struct GridSize size = {0};
        for( uint64_t l = 0; l < pages.top+1; l++ ) {
            printf("levelsize_x@%" PRIu64 ": %" PRIu64 "\n", l, grid_levelsize(&grid, &pages, l, &size)->x);
            printf("levelsize_y@%" PRIu64 ": %" PRIu64 "\n", l, grid_levelsize(&grid, &pages, l, &size)->y);
            printf("levelsize_z@%" PRIu64 ": %" PRIu64 "\n", l, grid_levelsize(&grid, &pages, l, &size)->z);
        }
    }
}

void grid_alloc(struct GridPages* pages, uint64_t page, uint32_t level) {
    struct GridSize size = {0};
    if( pages && pages->array ) {
        pages->array[page][level] = (Cell*)calloc(grid_pagesize(pages, level, &size)->array, sizeof(Cell));
    }
}

void grid_free(struct GridPages* pages, uint64_t page, uint32_t level) {
    if( pages && pages->array ) {
        free(pages->array[page][level]);
    }
}

void grid_clear(struct Grid* grid, struct GridPages* pages, struct GridBox* box) {
    if( NULL == box ) {
        box = &box_createN(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        struct GridIndex index = {0};
        for( uint64_t i = 0; i < array_size; i++ ) {
            grid_index(grid, pages, box, i, &index);
            pages->array[index.page][index.level][index.cell] = 0;
        }
    }
}

void grid_set1(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell) {
    if( NULL == box ) {
        box = &box_createN(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        struct GridIndex index = {0};
        for( uint64_t i = 0; i < array_size; i++ ) {
            grid_index(grid, pages, box, i, &index);
            //printf("%d %lu %lu %" PRIu64 "\n", i, index.page, index.level, index.cell);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] = cell;
            }
        }
    }
}


void grid_setN(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell* cells, uint64_t n) {
    if( NULL == box ) {
        box = &box_createN(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages && cells && n ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        uint64_t cell_i = 0;
        struct GridIndex index = {0};
        for( uint64_t i = 0; i < array_size; i++ ) {
            if( cell_i == n ) {
                cell_i = 0;
            }
            grid_index(grid, pages, box, i, &index);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] = cells[cell_i];
            }
            cell_i++;
        }
    }
}

void grid_and1(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell) {
    if( NULL == box ) {
        box = &box_createN(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        struct GridIndex index = {0};
        for( uint64_t i = 0; i < array_size; i++ ) {
            grid_index(grid, pages, box, i, &index);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] &= cell;
            }
        }
    }
}

void grid_andN(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell* cells, uint64_t n) {
    if( NULL == box ) {
        box = &box_createN(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages && cells && n ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        uint64_t cell_i = 0;
        struct GridIndex index = {0};
        for( uint64_t i = 0; i < array_size; i++ ) {
            if( cell_i == n ) {
                cell_i = 0;
            }
            grid_index(grid, pages, box, i, &index);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] &= cells[cell_i];
            }
            cell_i++;
        }
    }
}

void grid_or1(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell) {
    if( NULL == box ) {
        box = &box_createN(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        struct GridIndex index = {0};
        for( uint64_t i = 0; i < array_size; i++ ) {
            grid_index(grid, pages, box, i, &index);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] |= cell;
            }
        }
    }
}

void grid_orN(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell* cells, uint64_t n) {
    if( NULL == box ) {
        box = &box_createN(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages && cells && n ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        uint64_t cell_i = 0;
        struct GridIndex index = {0};
        for( uint64_t i = 0; i < array_size; i++ ) {
            if( cell_i == n ) {
                cell_i = 0;
            }
            grid_index(grid, pages, box, i, &index);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] |= cells[cell_i];
            }
            cell_i++;
        }
    }
}

void grid_shift(struct Grid* grid, struct GridPages* pages, struct GridBox* box, int32_t shift) {
    if( NULL == box ) {
        box = &box_createN(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages && shift ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        struct GridIndex index = {0};
        for( uint64_t i = 0; i < array_size; i++ ) {
            grid_index(grid, pages, box, i, &index);
            if( pages->array[index.page][index.level] ) {
                if( shift < 0 ) {
                    pages->array[index.page][index.level][index.cell] <<= abs(shift);
                } else {
                    pages->array[index.page][index.level][index.cell] >>= shift;
                }
            }
        }
    }
}

void grid_pagein(struct Grid* grid, struct GridPages* pages, uint64_t page, uint32_t level, char** in) {
}

void grid_pageout(struct Grid* grid, struct GridPages* pages, uint64_t page, uint32_t level, char** out) {
}

//  0    1| 2    3| 4    5
//    0   |   1   |   2
//  6    7| 8    9|10   11
// -------+-------+-------
// 12   13|14   15|16   17
//    3   |   4   |   5
// 18   19|20   21|22   23
// -------+-------+-------
// 24   25|26   27|28   29
//    6   |   7   |   8
// 30   31|32   33|34   35

// y * size.x * 4 + x
// y*size.x*4+x+1
// y*size.x*4+2*x
// y*size.x*4+2*x+1

// 0,0   1,0|2,0   3,0|4,0   5,0
//    0,0   |   1,0   |   2,0
// 0,1   1,1|2,1   3,1|4,1   5,1
// ---------+---------+---------
// 0,2   1,2|2,2   3,2|4,2   5,2
//    0,1   |   1,1   |   2,1
// 0,3   1,3|2,3   3,3|4,3   5,3
// ---------+---------+---------
// 0,4   1,4|2,4   3,4|4,4   5,4
//    0,2   |   1,2   |   2,2
// 0,5   1,5|2,5   3,5|4,5   5,5

void world_grid_create(struct Grid* grid,
                       struct GridPages* pages,
                       uint32_t level,
                       float width,
                       float height,
                       float depth,
                       struct Box* cube,
                       struct VboMesh* mesh)
{
    struct GridSize size = {0};
    grid_pagesize(pages, level, &size);

    uint64_t n = 12 * 3 * size.x * size.y * size.z;
    float* vertices = malloc(sizeof(float) * 3 * n);
    float* normals = malloc(sizeof(float) * 3 * n);
    uint8_t* colors = malloc(sizeof(uint8_t) * 4 * n);

    for( uint64_t zi = 0; zi < size.z; zi++ ) {
        for( uint64_t yi = 0; yi < size.y; yi++ ) {
            for( uint64_t xi = 0; xi < size.x; xi++ ) {
                float x = xi * width;
                float y = yi * height;
                float z = zi * depth;

                uint64_t offset = 12 * 3 * (zi * size.x * size.y + yi * size.x + xi);
                printf("%" PRIu64 " %" PRIu64 " %" PRIu64 " %f %f %f %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", xi, yi, zi, x, y, z, offset, cube->solid.size*3, n, pages->size.x);
                for( uint64_t i = 0; i < cube->solid.size; i++ ) {
                    vertices[(offset+i)*3+0] = x + cube->vertices[i*3+0]*width;
                    vertices[(offset+i)*3+1] = y + cube->vertices[i*3+1]*height;
                    vertices[(offset+i)*3+2] = z + cube->vertices[i*3+2]*depth;

                    normals[(offset+i)*3+0] = cube->normals[i*3+0];
                    normals[(offset+i)*3+1] = cube->normals[i*3+1];
                    normals[(offset+i)*3+2] = cube->normals[i*3+2];

                    colors[(offset+i)*4+0] = cube->colors[i*4+0];
                    colors[(offset+i)*4+1] = cube->colors[i*4+1];
                    colors[(offset+i)*4+2] = cube->colors[i*4+2];
                    colors[(offset+i)*4+3] = cube->colors[i*4+3];
                }
            }
        }
    }


    size_t vertices_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, n, vertices);
    size_t normals_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_NORMALS, 3, GL_FLOAT, n, normals);
    size_t colors_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE, n, colors);
    log_assert( vertices_n == n );
    log_assert( normals_n == n );
    log_assert( colors_n == n );

    free(vertices);
    free(normals);
    free(colors);
}

void world_grid_update(struct Grid* grid,
                       struct GridPages* pages,
                       uint32_t level,
                       uint64_t page,
                       struct Box* cube,
                       struct VboMesh* mesh)
{

    // first create _all_ vertices and normals in mesh needed for the grid, then just
    // generate the mesh by modifying the indices buffer

    // two ways to approach this:
    // 1. create a box for every cell of the grid
    //    + more straightforward to implement
    // 2. try to create only those faces that are neccessary
    //    + needs less bandwidth

    // I need to somehow make it possible to use the buffers that contain
    // the vertices and normals in multiple meshes if I want to be able
    // to use grid pages
    // 1. the best way to do this is to give this function the authority to
    //    create the meshes, given an vbo as input, an offset and a size as
    //    well as a page, this function then creates a mesh clone for that
    //    specific page
    // 2. could expect the user to supply meshes created with mesh_clone instead,
    //    maybe add some kind of field to mesh to 'lock' a mesh to make clones
    //    non-modifyable

    vbomesh_clear_indices(mesh);

    struct GridBox box = {0};
    grid_pagebox(grid, pages, page, level, &box);

    struct GridIndex index = {0};
    uint64_t n = 6 * 2 * box.size.x * box.size.y * box.size.z;
    uint64_t triangles[3 * n];
    for( uint64_t xi = 0; xi < box.size.x; xi++ ) {
        for( uint64_t yi = 0; yi < box.size.y; yi++ ) {
            for( uint64_t zi = 0; zi < box.size.z; zi++ ) {
                grid_index_xyz(grid, pages, &box, xi, yi, zi, &index);

                if( pages->array[index.page][index.level][index.cell] > 0 ) {
                    uint64_t v_offset = 3 * 12 * 3 * (zi * box.size.x * box.size.y + yi * box.size.x + xi);
                    uint64_t t_offset = 3 * 6 * 2 * (zi * box.size.x * box.size.y + yi * box.size.x + xi);
                    for( uint64_t i = 0; i < 36; i++ ) {
                        triangles[t_offset+i] = v_offset + cube->triangles[i];
                    }
                }
            }
        }
    }

    size_t triangles_n = vbomesh_append_indices(mesh, n, triangles);
    log_assert( triangles_n == n );
}
