#include "geometry.h"
#include "grid.h"
#include "solid.h"

void world_grid_create(struct Grid* grid,
                       struct GridPages* pages,
                       int level,
                       float width,
                       float height,
                       float depth,
                       struct Cube* cube,
                       struct VboMesh* mesh);
void world_grid_update(struct Grid* grid,
                       struct GridPages* pages,
                       int level,
                       uint64_t page,
                       struct Cube* cube,
                       struct VboMesh* mesh);
