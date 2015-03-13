#include "world.h"

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
                       int level,
                       float width,
                       float height,
                       float depth,
                       struct Cube* cube,
                       struct Mesh* mesh)
{
    struct GridSize size;
    grid_pagesize(pages, level, &size);

    uint64_t n = 12 * 3 * size.x * size.y * size.z;
    GLfloat vertices[3 * n];
    GLfloat normals[3 * n];
    GLfloat colors[4 * n];

    for( uint64_t zi = 0; zi < size.z; zi++ ) {
        for( uint64_t yi = 0; yi < size.y; yi++ ) {
            for( uint64_t xi = 0; xi < size.x; xi++ ) {
                GLfloat x = xi * width;
                GLfloat y = yi * height;
                GLfloat z = zi * depth;

                uint64_t offset = 12 * 3 * (zi * size.x * size.y + yi * size.x + xi);
                printf("%lu %lu %lu %f %f %f %lu %d %lu %lu\n", xi, yi, zi, x, y, z, offset, cube->solid.size*3, n, pages->size.x);
                for( int i = 0; i < cube->solid.size; i++ ) {
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


    mesh_append(mesh, VERTEX_ARRAY, vertices, n);
    mesh_append(mesh, NORMAL_ARRAY, normals, n);
    mesh_append(mesh, COLOR_ARRAY, colors, n);
}

void world_grid_update(struct Grid* grid,
                       struct GridPages* pages,
                       int level,
                       uint64_t page,
                       struct Cube* cube,
                       struct Mesh* mesh)
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

    mesh_clear_primitives(mesh);

    struct GridBox box;
    grid_pagebox(grid, pages, page, level, &box);

    struct GridIndex index;
    uint64_t n = 6 * 2 * box.size.x * box.size.y * box.size.z;
    GLuint triangles[3 * n];
    for( uint64_t xi = 0; xi < box.size.x; xi++ ) {
        for( uint64_t yi = 0; yi < box.size.y; yi++ ) {
            for( uint64_t zi = 0; zi < box.size.z; zi++ ) {
                grid_index_xyz(grid, pages, &box, xi, yi, zi, &index);

                if( pages->array[index.page][index.level][index.cell] > 0 ) {
                    uint64_t v_offset = 3 * 12 * 3 * (zi * box.size.x * box.size.y + yi * box.size.x + xi);
                    uint64_t t_offset = 3 * 6 * 2 * (zi * box.size.x * box.size.y + yi * box.size.x + xi);
                    for( int i = 0; i < 36; i++ ) {
                        triangles[t_offset+i] = v_offset + cube->triangles[i];
                    }
                }
            }
        }
    }

    mesh_primitives(mesh, triangles, n);
}
