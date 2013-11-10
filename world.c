#include "world.h"

void world_gridmesh(struct Grid* grid, struct GridPages* pages, struct Mesh* mesh) {
    struct GridSize size;
    grid_size(grid, pages, &size, 0);
    
    struct GridIndex index;
    for( uint64_t x = 0; x < size.x; x++ ) {
        for( uint64_t y = 0; y < size.y; y++ ) {
            for( uint64_t z = 0; z < size.z; z++ ) {
                grid_xyz(grid, pages, NULL, &index, x, y, z);
                if( pages->array[index.page][index.level][index.cell] > 0 ) {
                    // add cube to mesh
                }
            }
        }
    }
}
