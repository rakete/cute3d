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

#include "geometry_halfedgemesh.h"

void halfedgemesh_create(struct HalfEdgeMesh* mesh) {
    mesh->size = 0;

    mesh->vertices.capacity = 0;
    mesh->vertices.occupied = 0;
    mesh->vertices.array = NULL;

    mesh->faces.capacity = 0;
    mesh->faces.occupied = 0;
    mesh->faces.array = NULL;

    mesh->edges.capacity = 0;
    mesh->edges.occupied = 0;
    mesh->edges.array = NULL;
}

void halfedgemesh_destroy(struct HalfEdgeMesh* mesh) {
    free(mesh->vertices.array);
    free(mesh->faces.array);
    free(mesh->edges.array);

    halfedgemesh_create(mesh);
}

size_t halfedgemesh_alloc_vertices(struct HalfEdgeMesh* mesh, size_t n) {
    if( n == 0 ) {
        return 0;
    }

    size_t vertices_alloc = HALFEDGEMESH_VERTICES_ALLOC;
    while( vertices_alloc < n ) {
        vertices_alloc += HALFEDGEMESH_VERTICES_ALLOC;
    }

    size_t new_capacity = mesh->vertices.capacity + vertices_alloc;
    struct HalfEdgeVertex* new_array = realloc(mesh->vertices.array, new_capacity * sizeof(struct HalfEdgeVertex));

    if( new_array ) {
        mesh->vertices.array = new_array;
        mesh->vertices.capacity = new_capacity;
        return vertices_alloc;
    }

    return 0;
}

size_t halfedgemesh_alloc_faces(struct HalfEdgeMesh* mesh, size_t n) {
    if( n == 0 ) {
        return 0;
    }

    size_t faces_alloc = HALFEDGEMESH_FACES_ALLOC;
    while( faces_alloc < n ) {
        faces_alloc += HALFEDGEMESH_FACES_ALLOC;
    }

    size_t new_capacity = mesh->faces.capacity + faces_alloc;
    struct HalfEdgeFace* new_array = realloc(mesh->faces.array, new_capacity * sizeof(struct HalfEdgeFace));

    if( new_array ) {
        mesh->faces.array = new_array;
        mesh->faces.capacity = new_capacity;
        return faces_alloc;
    }

    return 0;
}


size_t halfedgemesh_alloc_edges(struct HalfEdgeMesh* mesh, size_t n) {
    if( n == 0 ) {
        return 0;
    }

    size_t edges_alloc = HALFEDGEMESH_EDGES_ALLOC;
    while( edges_alloc < n ) {
        edges_alloc += HALFEDGEMESH_EDGES_ALLOC;
    }

    size_t new_capacity = mesh->edges.capacity + edges_alloc;
    struct HalfEdge* new_array = realloc(mesh->edges.array, new_capacity * sizeof(struct HalfEdge));

    if( new_array ) {
        mesh->edges.array = new_array;
        mesh->edges.capacity = new_capacity;
        return edges_alloc;
    }

    return 0;
}

void halfedgemesh_append(struct HalfEdgeMesh* mesh, const struct Solid* solid) {
    log_assert( solid->indices_size == solid->attributes_size, "don't append an optimized or compressed solid to a halfedgemesh\n" );

    log_assert( solid->indices_size > 0 );
    log_assert( solid->indices_size < INT_MAX/3 );
    log_assert( solid->indices != NULL );
    log_assert( solid->triangles != NULL );
    log_assert( solid->vertices != NULL );
    log_assert( solid->normals != NULL );
    log_assert( solid->colors != NULL );

    // this function needs to identify all unique vertices of the given solid, add those
    // to the mesh, then go through all triangles of the solid and create all faces and
    // half edges (in correct order, which for now we'll assume is the order in which they
    // are given in the solid, we could check with crossproduct and comparison with normals)
    //
    // to identify unique vertices we'll use their triangle indices, if two vertices have
    // the same integer in solid->triangles, they should be the same vertex
    //
    // so this loop initializes a map that is later used with an integer from solid->triangles
    // as index to look up if a vertex has already been seen
    //
    // for example we process the triangle 1 2 3 first, each of those would be checked in
    // vertex_map[1,2,3] == -1 and since these are all unique they would be stored in
    // the mesh and that new mesh index would be written into vertex_map[1,2,3] = new_index,
    // then if we processed triangle 2 3 4, we'd know that only 4 would be unique and needs
    // to be stored, but 2 3 would already be in vertex_map and we could even find their
    // respective new indices where they are stored in the mesh
    //
    // we also use this loop to find the largest vertex index from solid->triangles, since
    // each unique vertex should be represented by one unique index, the largest index + 1
    // should be the number of unique vertices
    int32_t* unique_vertex_map = malloc(sizeof(int32_t) * solid->indices_size);
    log_assert( unique_vertex_map != NULL );

    uint32_t num_unique_vertices = 0;
    for( uint32_t i = 0; i < solid->indices_size; i++ ) {
        unique_vertex_map[i] = -1;

        if( solid->triangles[i] > num_unique_vertices || i == 0 ) {
            num_unique_vertices = solid->triangles[i];
        }
    }
    num_unique_vertices += 1;

    // the edges_map is initialized like the unique_vertex_map and used in a similar way, to be used
    // later when we need to decide if an edge has already been seen so that we can set the
    // correct other indices
    size_t num_triangles = solid->indices_size/3;

    //int32_t edges_map[num_unique_vertices][num_unique_vertices];
    int32_t* edges_map = malloc(sizeof(int32_t) * num_unique_vertices*num_unique_vertices);
    log_assert( edges_map != NULL );

    for( uint32_t i = 0; i < num_triangles; i++ ) {
        uint32_t a = solid->triangles[i*3+0];
        uint32_t b = solid->triangles[i*3+1];
        uint32_t c = solid->triangles[i*3+2];

        edges_map[a * num_unique_vertices + b] = -1;
        edges_map[a * num_unique_vertices + c] = -1;

        edges_map[b * num_unique_vertices + a] = -1;
        edges_map[b * num_unique_vertices + c] = -1;

        edges_map[c * num_unique_vertices + a] = -1;
        edges_map[c * num_unique_vertices + b] = -1;
    }

    // - we need to make sure that we'll have enough space occupied in the mesh for all
    // the data that we are going to add
    // - this is lacking error handling
    size_t free_vertices_capacity = mesh->vertices.capacity - mesh->vertices.occupied;
    if( free_vertices_capacity < num_unique_vertices ) {
        size_t alloc_vertices_n = num_unique_vertices - free_vertices_capacity;
        size_t alloc_vertices_result = halfedgemesh_alloc_vertices(mesh, alloc_vertices_n);
        log_assert( alloc_vertices_result >= alloc_vertices_n );
    }

    size_t free_faces_capacity = mesh->faces.capacity - mesh->faces.occupied;
    if( free_faces_capacity < num_triangles ) {
        size_t alloc_faces_n = num_triangles - free_faces_capacity;
        size_t alloc_faces_result = halfedgemesh_alloc_faces(mesh, alloc_faces_n);
        log_assert( alloc_faces_result >= alloc_faces_n );
    }

    size_t free_edges_capacity = mesh->edges.capacity - mesh->edges.occupied;
    if( free_edges_capacity < num_triangles*3 ) {
        size_t alloc_edges_n = num_triangles*3 - free_edges_capacity;
        size_t alloc_edges_result = halfedgemesh_alloc_edges(mesh, alloc_edges_n);
        log_assert( alloc_edges_result >= alloc_edges_n );
    }

    int32_t vertex_i = mesh->vertices.occupied;
    int32_t face_i = mesh->faces.occupied;
    int32_t edge_i = mesh->edges.occupied;
    for( uint32_t i = 0; i < num_triangles; i++ ) {

        // mainloop iterates once over all triangles, then iterates over the vertices of a triangle,
        // collects each vertex normal, checks if the vertex is unique by looking up its triangle index in
        // unique_vertex_map, and if it is unique it adds it to the mesh and increases vertex_i, the
        // counter for vertices added to the mesh
        float normals[9] = {0};
        uint8_t colors[12] = {0};
        float texcoords[6] = {0};
        uint32_t triangle[3] = {
            solid->triangles[i*3+0],
            solid->triangles[i*3+1],
            solid->triangles[i*3+2]
        };

        for( uint32_t j = 0; j < 3; j++ ) {
            uint32_t solid_i = i*3+j;
            normals[j*3+0] = solid->normals[solid_i*3+0];
            normals[j*3+1] = solid->normals[solid_i*3+1];
            normals[j*3+2] = solid->normals[solid_i*3+2];

            colors[j*4+0] = solid->colors[solid_i*4+0];
            colors[j*4+1] = solid->colors[solid_i*4+1];
            colors[j*4+2] = solid->colors[solid_i*4+2];
            colors[j*4+3] = solid->colors[solid_i*4+3];

            texcoords[j*2+0] = solid->texcoords[solid_i*2+0];
            texcoords[j*2+1] = solid->texcoords[solid_i*2+1];

            if( unique_vertex_map[triangle[j]] == -1 ) {
                unique_vertex_map[triangle[j]] = vertex_i;

                mesh->vertices.array[vertex_i].position[0] = solid->vertices[solid_i*3+0];
                mesh->vertices.array[vertex_i].position[1] = solid->vertices[solid_i*3+1];
                mesh->vertices.array[vertex_i].position[2] = solid->vertices[solid_i*3+2];
                mesh->vertices.array[vertex_i].position[3] = 1.0;

                // after I changed this function to put the halfedges in the array so that every
                // second one belongs to an unique edge, I could not compute the edge index at this
                // point, so this gets set to -1 and then I set it later
                mesh->vertices.array[vertex_i].edge = -1;

                // make vertex refer to itself
                mesh->vertices.array[vertex_i].this = vertex_i;

                vertex_i += 1;
            }
        }

        // convience names for the three vertex indices from triangle that we'll use to represent the halfedges
        // a -> b, b -> c and c -> a
        uint32_t a = triangle[0];
        uint32_t b = triangle[1];
        uint32_t c = triangle[2];
        log_assert( a != b && b != c && c != a );

        // we assume each pair of vertex indices, representing an edge, gets seen once as a -> b and once again
        // as b -> a (which conveniently fits our model of representing each edge as two half edges pointing
        // into opposite directions)
        //
        // so we assert that halfedge index for the current halfedge has never been set in edges_map
        // and should still be -1
        log_assert( edges_map[a * num_unique_vertices + b] == -1 );
        log_assert( edges_map[b * num_unique_vertices + c] == -1 );
        log_assert( edges_map[c * num_unique_vertices + a] == -1 );

        // indices for the three new halfedges we'll create in the mesh
        int32_t ca_i = -1;
        int32_t ab_i = -1;
        int32_t bc_i = -1;

        // if we had already seen the current edge in a previous iteration, then with flipped indices (in this
        // a -> b case we would have seen b -> a previously), and we can check that by testing if edges_map[b][a]
        // has already been set to something other then -1
        int32_t ab_other_i = -1;
        if( edges_map[b * num_unique_vertices + a] != -1 ) {
            // restore the other index from the already seen edge
            ab_other_i = edges_map[b * num_unique_vertices + a];
            // I want to order the edges so that every second halfedge is from a unique edge, so since we have
            // already seen the b -> a edge, we put this halfedge at an index that comes right after the index
            // of the already seen halfedge
            ab_i = ab_other_i + 1;
            // set the other index of the already seen halfedge, this is not strictly neccessary here, we could
            // just set the other index to this + 1 when creating the first halfedge of an new edge, but I kept
            // it this way so that if we ever get a mesh with holes in it, the other index will be -1 for
            // some halfedges, indicating that there is no other halfedge
            mesh->edges.array[ab_other_i].other = ab_i;
        }

        // these two are the same as the ab case above, but for bc and ca
        int32_t bc_other_i = -1;
        if( edges_map[c * num_unique_vertices + b] != -1 ) {
            bc_other_i = edges_map[c * num_unique_vertices + b];
            bc_i = bc_other_i + 1;
            mesh->edges.array[bc_other_i].other = bc_i;
        }

        int32_t ca_other_i = -1;
        if( edges_map[a * num_unique_vertices + c] != -1 ) {
            ca_other_i = edges_map[a * num_unique_vertices + c];
            ca_i = ca_other_i + 1;
            mesh->edges.array[ca_other_i].other = ca_i;
        }

        // the above set ab_i, bc_i and ca_i indices in case the edge was already seen, but if it is new
        // I have to set the indices of the halfedges so that they are spaced out by 2, leaving space for
        // the other halfedge to be added later
        // now I may have a situation where all edges are new, but most likely one, two or all three of them
        // are _not_ new, so I have to account for that and set only those indices to newly spaced indices
        // that belong to actually new edges
        int32_t edge_i_inc = 0;
        if( ca_i == -1 ) {
            // when ca_i is -1, it was not set in the if conditionals above, and therefore belongs
            // to a new (unique) edge, so we set it to edge_i + edge_i_inc and then increase edge_i by 2
            // so that we leave space for the other halfedge of ca_i to come later and fill that space
            ca_i = edge_i + edge_i_inc;
            // edge_i_inc keeps track of how many new halfedges indices we added, it will be either 0,2,4 or 6
            // depeding on if we added none, one, two or three new edges
            edge_i_inc += 2;

            // it may be possible that we never actually set the other halfedge, so I am just goint
            // to initialize the other halfedge with everything set to -1 just so that I can be
            // sure that is does not actually look like a legit halfedge
            struct HalfEdge* other = &mesh->edges.array[ca_i+1];
            other->vertex = -1;
            other->face = -1;
            other->next = -1;
            other->this = -1;
            other->prev = -1;
            other->other = -1;
        }

        if( ab_i == -1 ) {
            ab_i = edge_i + edge_i_inc;
            edge_i_inc += 2;

            struct HalfEdge* other = &mesh->edges.array[ab_i+1];
            other->vertex = -1;
            other->face = -1;
            other->next = -1;
            other->this = -1;
            other->prev = -1;
            other->other = -1;
        }

        if( bc_i == -1 ) {
            bc_i = edge_i + edge_i_inc;
            edge_i_inc += 2;

            struct HalfEdge* other = &mesh->edges.array[bc_i+1];
            other->vertex = -1;
            other->face = -1;
            other->next = -1;
            other->this = -1;
            other->prev = -1;
            other->other = -1;
        }

        log_assert( ab_i != -1 );
        log_assert( bc_i != -1 );
        log_assert( ca_i != -1 );

        // we need to set the halfedges index in the edges_map so we can use it as other index when we'll see the
        // current edge again in the future
        edges_map[a * num_unique_vertices + b] = ab_i;
        edges_map[b * num_unique_vertices + c] = bc_i;
        edges_map[c * num_unique_vertices + a] = ca_i;

        // finally construct the halfedge for a -> b, b -> c and c -> a
        struct HalfEdge* ab_ptr = &mesh->edges.array[ab_i];
        *ab_ptr = (struct HalfEdge){
            .normal[0] = normals[3],
            .normal[1] = normals[4],
            .normal[2] = normals[5],
            .color[0] = colors[4],
            .color[1] = colors[5],
            .color[2] = colors[6],
            .color[3] = colors[7],
            .texcoord[0] = texcoords[2],
            .texcoord[1] = texcoords[3],
            .vertex = unique_vertex_map[b],
            .face = face_i,
            .next = bc_i,
            .this = ab_i,
            .prev = ca_i,
            .other = ab_other_i
        };

        struct HalfEdge* bc_ptr = &mesh->edges.array[bc_i];
        *bc_ptr = (struct HalfEdge){
            .normal[0] = normals[6],
            .normal[1] = normals[7],
            .normal[2] = normals[8],
            .color[0] = colors[8],
            .color[1] = colors[9],
            .color[2] = colors[10],
            .color[3] = colors[11],
            .texcoord[0] = texcoords[4],
            .texcoord[1] = texcoords[5],
            .vertex = unique_vertex_map[c],
            .face = face_i,
            .next = ca_i,
            .this = bc_i,
            .prev = ab_i,
            .other = bc_other_i
        };

        struct HalfEdge* ca_ptr = &mesh->edges.array[ca_i];
        *ca_ptr = (struct HalfEdge){
            .normal[0] = normals[0],
            .normal[1] = normals[1],
            .normal[2] = normals[2],
            .color[0] = colors[0],
            .color[1] = colors[1],
            .color[2] = colors[2],
            .color[3] = colors[3],
            .texcoord[0] = texcoords[0],
            .texcoord[1] = texcoords[1],
            .vertex = unique_vertex_map[a],
            .face = face_i,
            .next = ab_i,
            .this = ca_i,
            .prev = bc_i,
            .other = ca_other_i
        };

        // above I could not compute the neccessary edge index to set in the vertices array as outgoing,
        // now I know it
        // I only set it if the vertex has not yet an outgoing edge set
        if( mesh->vertices.array[unique_vertex_map[a]].edge ) {
            mesh->vertices.array[unique_vertex_map[a]].edge = ab_i;
        }

        if( mesh->vertices.array[unique_vertex_map[b]].edge ) {
            mesh->vertices.array[unique_vertex_map[b]].edge = bc_i;
        }

        if( mesh->vertices.array[unique_vertex_map[c]].edge ) {
            mesh->vertices.array[unique_vertex_map[c]].edge = ca_i;
        }

        // after creating the halfedges the face gets set, this just as simple as it looks since the face
        // contains nothing but an index to any of its halfedges
        struct HalfEdgeFace* face_ptr = &mesh->faces.array[face_i];
        face_ptr->size = 3;
        face_ptr->edge = ca_i;
        face_ptr->this = face_i;

        Vec4f vec_a = {0};
        Vec4f vec_b = {0};
        vec_sub(mesh->vertices.array[unique_vertex_map[a]].position, mesh->vertices.array[unique_vertex_map[b]].position, vec_a);
        vec_sub(mesh->vertices.array[unique_vertex_map[b]].position, mesh->vertices.array[unique_vertex_map[c]].position, vec_b);
        /* vec_normalize(vec_a, vec_a); */
        /* vec_normalize(vec_b, vec_b); */
        vec_cross(vec_a, vec_b, face_ptr->normal);
        vec_normalize(face_ptr->normal, face_ptr->normal);

        // we need to keep track about how many faces and edges we added to the mesh with
        // these index counters that indicate the position where we'll add the next face/edge
        // when the iteration continues to the next triangle
        face_i += 1;
        // edge_i needs to be only increased by the amount of new edges we added
        edge_i += edge_i_inc;

        mesh->size += edge_i_inc;

        log_assert( vertex_i > 0 );
        log_assert( mesh->vertices.capacity < INT32_MAX );
        log_assert( vertex_i <= (int32_t)mesh->vertices.capacity );
        mesh->vertices.occupied = (size_t)vertex_i;

        log_assert( face_i > 0 );
        log_assert( mesh->faces.capacity < INT32_MAX );
        log_assert( face_i <= (int32_t)mesh->faces.capacity );
        mesh->faces.occupied = (size_t)face_i;

        log_assert( edge_i > 0 );
        log_assert( mesh->edges.capacity < INT32_MAX );
        log_assert( edge_i <= (int32_t)mesh->edges.capacity );
        mesh->edges.occupied = (size_t)edge_i;
    }

    free(unique_vertex_map);
    free(edges_map);
}

int32_t halfedgemesh_face_normal(const struct HalfEdgeMesh* mesh, int32_t face_i, int32_t all_edges, Vec3f equal_normal, Vec3f average_normal) {
    log_assert( mesh != NULL );
    log_assert( mesh->faces.occupied < INT32_MAX );
    log_assert( face_i >= 0 );
    log_assert( face_i <= (int32_t)mesh->faces.occupied );
    log_assert( equal_normal != NULL || average_normal != NULL );

    struct HalfEdgeFace* face = &mesh->faces.array[face_i];

    struct HalfEdge* first_edge = &mesh->edges.array[face->edge];
    struct HalfEdge* current_edge = &mesh->edges.array[face->edge];

    int32_t result = 0;
    if( equal_normal != NULL ) {
        vec_copy3f(first_edge->normal, equal_normal);
    }

    if( average_normal != NULL ) {
        vec_copy3f(first_edge->normal, average_normal);
        result = 1;
    }

    if( all_edges && (equal_normal != NULL || average_normal != NULL) ) {
        for( int32_t i = 0; i < face->size-1; i++ ) {
            if( equal_normal != NULL && vequal(first_edge->normal, current_edge->normal) ) {
                result = 1;
            }

            if( average_normal != NULL ) {
                average_normal[0] += current_edge->normal[0];
                average_normal[1] += current_edge->normal[1];
                average_normal[2] += current_edge->normal[2];
            } else if( result == 0 ) {
                break;
            }

            current_edge = &mesh->edges.array[current_edge->next];
        }

        if( average_normal != NULL ) {
            average_normal[0] /= face->size;
            average_normal[1] /= face->size;
            average_normal[2] /= face->size;

            if( vlength(average_normal) > CUTE_EPSILON ) {
                result = 1;
            }
        }
    }

    return result;
}

int32_t halfedgemesh_face_iterate(const struct HalfEdgeMesh* mesh, int32_t face_i, struct HalfEdge** edge, int32_t* edge_i, int32_t* i) {
    log_assert( mesh != NULL );
    log_assert( mesh->faces.occupied < INT32_MAX );
    log_assert( face_i >= 0 );
    log_assert( face_i <= (int32_t)mesh->faces.occupied );
    log_assert( edge != NULL );
    log_assert( i != NULL );
    log_assert( edge_i != NULL );

    struct HalfEdgeFace* face = &mesh->faces.array[face_i];

    if( *i == 0 || *edge_i == -1 ) {
        *edge_i = mesh->faces.array[face_i].edge;
    } else {
        *edge_i = mesh->edges.array[*edge_i].next;
    }
    *edge = &mesh->edges.array[*edge_i];

    if( *i == face->size ) {
        *i = 0;
        return 0;
    }
    *i += 1;
    return 1;
}

int32_t halfedgemesh_vertex_iterate(const struct HalfEdgeMesh* mesh, int32_t vertex_i, struct HalfEdge** edge, int32_t* edge_i, int32_t* i) {
    log_assert( mesh != NULL );
    log_assert( mesh->vertices.occupied < INT32_MAX );
    log_assert( vertex_i >= 0 );
    log_assert( vertex_i <= (int32_t)mesh->vertices.occupied );
    log_assert( edge != NULL );
    log_assert( edge_i != NULL );
    log_assert( i != NULL );

    int32_t result = 0;
    if( *i == 0 || *edge_i == -1 ) {
        *edge_i = mesh->vertices.array[vertex_i].edge;
        result = 1;
    } else if( *i % 2 == 1 ) {
        *edge_i = mesh->edges.array[*edge_i].other;
        log_assert( mesh->edges.array[*edge_i].vertex == vertex_i );
        result = -1;
    } else if( *i % 2 == 0 ) {
        *edge_i = mesh->edges.array[*edge_i].next;
        result = 1;
    }
    *edge = &mesh->edges.array[*edge_i];

    if( *i > 0 && *edge_i == mesh->vertices.array[vertex_i].edge ) {
        *i = 0;
        return 0;
    }
    *i += 1;
    return result;
}

void halfedgemesh_optimize(struct HalfEdgeMesh* mesh) {
    log_assert( mesh != NULL );
    log_assert( mesh->vertices.occupied < INT32_MAX );
    log_assert( mesh->edges.occupied < INT32_MAX );
    log_assert( mesh->faces.occupied < INT32_MAX );

    // for all edges (_both_ halfedges), check if the face normals
    // on both sides are equal, if they are, mark this edge for removal
    //
    // after checking all edges, remove those edges marked for removal,
    // remove all faces marked for removal
    int32_t* edge_check = malloc(sizeof(int32_t) * mesh->edges.occupied);
    log_assert( edge_check != NULL );

    for( int32_t i = 0; i < (int32_t)mesh->edges.occupied; i++ ) {
        edge_check[i] = 1;
    }

    log_assert(mesh->size >= 0);
    log_assert(mesh->size % 3 == 0);
    int32_t* face_has_normal = malloc(sizeof(int32_t) * (size_t)(mesh->size/3));
    log_assert( face_has_normal != NULL );

    float* face_normals = malloc(sizeof(float) * (size_t)mesh->size);
    log_assert( face_normals != NULL );

    for( int32_t i = 0; i < mesh->size; i++ ) {
        if( i < mesh->size/3 ) {
            face_has_normal[i] = 0;
        }
        face_normals[i] = 0.0f;
    }

    int32_t* removed_faces = malloc(sizeof(int32_t) * mesh->faces.occupied);
    log_assert( removed_faces != NULL );
    int32_t num_removed_faces = 0;

    int32_t* removed_edges = malloc(sizeof(int32_t) * mesh->edges.occupied);
    log_assert( removed_edges != NULL );
    int32_t num_removed_edges = 0;

    int32_t* removed_vertices = malloc(sizeof(int32_t) * mesh->vertices.occupied);
    log_assert( removed_vertices != NULL );
    int32_t num_removed_vertices = 0;

    int32_t* attached_edges = malloc(sizeof(int32_t) * mesh->vertices.occupied);
    log_assert( attached_edges != NULL );

    for( int32_t i = 0; i < (int32_t)mesh->vertices.occupied; i++ ) {
        attached_edges[i] = 0;
    }

    for( int32_t this_i = 0; this_i < (int32_t)mesh->edges.occupied; this_i++ ) {
        if( edge_check[this_i] > 0 ) {
            int32_t other_i = mesh->edges.array[this_i].other;
            edge_check[this_i] = 0;
            edge_check[other_i] = 0;

            int32_t face_one_i = mesh->edges.array[this_i].face;
            int32_t face_two_i = mesh->edges.array[other_i].face;

            if( ! face_has_normal[face_one_i] ) {
                face_has_normal[face_one_i] = halfedgemesh_face_normal(mesh, face_one_i, 1, &face_normals[face_one_i*3], NULL);
            }

            if( ! face_has_normal[face_two_i] ) {
                face_has_normal[face_two_i] = halfedgemesh_face_normal(mesh, face_two_i, 1, &face_normals[face_two_i*3], NULL);
            }

            struct HalfEdge* this = &mesh->edges.array[this_i];
            struct HalfEdge* other = &mesh->edges.array[other_i];
            attached_edges[this->vertex] += 1;
            attached_edges[other->vertex] += 1;

            if( face_has_normal[face_one_i] && face_has_normal[face_two_i] ) {
                Vec4f normal_a = {0};
                normal_a[0] = face_normals[face_one_i*3+0];
                normal_a[1] = face_normals[face_one_i*3+1];
                normal_a[2] = face_normals[face_one_i*3+2];

                Vec4f normal_b = {0};
                normal_b[0] = face_normals[face_two_i*3+0];
                normal_b[1] = face_normals[face_two_i*3+1];
                normal_b[2] = face_normals[face_two_i*3+2];

                if( vequal(normal_a, normal_b) ) {
                    log_assert( this->prev != this->this );
                    log_assert( this->next != this->this );
                    log_assert( this->prev != this->next );

                    log_assert( other->prev != other->this );
                    log_assert( other->next != other->this );
                    log_assert( other->prev != other->next );

                    log_assert( other->prev != this->this );
                    log_assert( other->next != this->this );
                    log_assert( other->prev != this->next );

                    log_assert( this->prev != other->this );
                    log_assert( this->next != other->this );
                    log_assert( this->prev != other->next );

                    int32_t halfedge_a_i = this->next;
                    struct HalfEdge* halfedge_a = &mesh->edges.array[halfedge_a_i];

                    int32_t halfedge_b_i = other->prev;
                    struct HalfEdge* halfedge_b = &mesh->edges.array[halfedge_b_i];

                    int32_t halfedge_c_i = this->prev;
                    struct HalfEdge* halfedge_c = &mesh->edges.array[halfedge_c_i];

                    int32_t halfedge_d_i = other->next;
                    struct HalfEdge* halfedge_d = &mesh->edges.array[halfedge_d_i];

                    // a\  /b
                    //   +i
                    // 1 |e 2
                    //   +j
                    // c/  \d

                    // to mark an edge e for removal:

                    // - check if this edge was the last edge removed attached to vertices i and j,
                    // and remove i or j or both if it were the last edge attached to them
                    // - I could remember how many edges are attached to a vertex the first time I see it, then
                    // count down every time I remove an edge that is attached to it, when I hit zero I know
                    // that I can delete the vertex
                    attached_edges[this->vertex] -= 1;
                    attached_edges[other->vertex] -= 1;

                    // - change all edges in face 2 so that they belong to 1,
                    struct HalfEdge* iter_edge = NULL;
                    int32_t iter_edge_i = -1;
                    int32_t iter_i = 0;
                    while( halfedgemesh_face_iterate(mesh, face_two_i, &iter_edge, &iter_edge_i, &iter_i) ) {
                        iter_edge->face = face_one_i;
                    }

                    // increase face size of face one by face size of face size, - 2 because we are deleting an
                    // edge (two halfedges, one from each face)
                    log_assert( mesh->faces.array[face_one_i].size > 2 && mesh->faces.array[face_two_i].size > 2 );
                    mesh->faces.array[face_one_i].size += mesh->faces.array[face_two_i].size - 2;
                    // mark face 2 for removal
                    mesh->faces.array[face_two_i].edge = -1;
                    removed_faces[num_removed_faces] = face_two_i;
                    num_removed_faces += 1;

                    // - if this halfedge is the face edge of face_one, change the face edge to the next
                    // edge
                    if( mesh->faces.array[face_one_i].edge == this->this ) {
                        mesh->faces.array[face_one_i].edge = this->next;
                    }

                    // - update a's next to b, and b's prev to a, and c's next to d, and d's prev to c
                    log_assert( halfedge_a->prev == this->this );
                    log_assert( halfedge_b->next == other->this );
                    log_assert( halfedge_c->next == this->this );
                    log_assert( halfedge_d->prev == other->this );

                    halfedge_a->prev = halfedge_b_i;
                    halfedge_b->next = halfedge_a_i;
                    halfedge_c->next = halfedge_d_i;
                    halfedge_d->prev = halfedge_c_i;

                    // - if e was outgoing edge of i, change it to a
                    if( mesh->vertices.array[this->vertex].edge == other_i ) {
                        mesh->vertices.array[this->vertex].edge = halfedge_a_i;
                    }
                    // - if e was outgoing edge of j, change it to d
                    if( mesh->vertices.array[other->vertex].edge == this_i ) {
                        mesh->vertices.array[other->vertex].edge = halfedge_d_i;
                    }

                    this->face = -1;
                    other->face = -1;
                    removed_edges[num_removed_edges] = this_i;
                    num_removed_edges += 1;
                    removed_edges[num_removed_edges] = other_i;
                    num_removed_edges += 1;

                    log_assert( halfedge_a->prev != halfedge_a->this );
                    log_assert( halfedge_a->next != halfedge_a->this );
                    log_assert( halfedge_a->prev != halfedge_a->next );

                    log_assert( halfedge_b->prev != halfedge_b->this );
                    log_assert( halfedge_b->next != halfedge_b->this );
                    log_assert( halfedge_b->prev != halfedge_b->next );

                    log_assert( halfedge_c->prev != halfedge_c->this );
                    log_assert( halfedge_c->next != halfedge_c->this );
                    log_assert( halfedge_c->prev != halfedge_c->next );

                    log_assert( halfedge_d->prev != halfedge_d->this );
                    log_assert( halfedge_d->next != halfedge_d->this );
                    log_assert( halfedge_d->prev != halfedge_d->next );

                    log_assert( this->prev != this->this );
                    log_assert( this->next != this->this );
                    log_assert( this->prev != this->next );

                    log_assert( other->prev != other->this );
                    log_assert( other->next != other->this );
                    log_assert( other->prev != other->next );

                    log_assert( other->prev != this->this );
                    log_assert( other->next != this->this );
                    log_assert( other->prev != this->next );

                    log_assert( this->prev != other->this );
                    log_assert( this->next != other->this );
                    log_assert( this->prev != other->next );
                }
            }
        }
    }

    for( int32_t i = 0; i < (int32_t)mesh->vertices.occupied; i++ ) {
        if( attached_edges[i] <= 0 ) {
            mesh->vertices.array[i].edge = -1;
            removed_vertices[num_removed_vertices] = i;
            num_removed_vertices++;
        }
    }

    // after putting everthing that needs to be deleted into removed_vertices, removed_edges and removed_faces,
    // I need to actually delete those from the arrays in struct HalfEdgeMesh* mesh
    // to do that I need to go through all arrays, and fill all gaps left by deleted primitives
    // with the primitives after, that still exist
    int32_t iter_vertex = 0;
    int32_t iter_face = 0;
    int32_t iter_edge = 0;

    int32_t iter_removed_vertices = 0;
    int32_t iter_removed_faces = 0;
    int32_t iter_removed_edges = 0;

    int32_t gap_vertices = 0;
    int32_t gap_faces = 0;
    int32_t gap_edges = 0;

    // this loop is bad and I don't like it, but it seems to work so it will stay like this anyways
    while( (num_removed_vertices && iter_vertex < (int32_t)mesh->vertices.occupied) ||
           (num_removed_faces && iter_face < (int32_t)mesh->faces.occupied) ||
           (num_removed_edges && iter_edge < (int32_t)mesh->edges.occupied) )
    {
        // deleting vertices is untested
        if( num_removed_vertices && iter_vertex < (int32_t)mesh->vertices.occupied ) {
            while( iter_removed_vertices < num_removed_vertices && removed_vertices[iter_removed_vertices] == iter_vertex ) {
                gap_vertices++;
                iter_vertex++;
                iter_removed_vertices++;
            }

            if( gap_vertices > 0 && iter_vertex < (int32_t)mesh->vertices.occupied ) {
                int32_t old_vertex_i = iter_vertex;
                int32_t new_vertex_i = iter_vertex - gap_vertices;

                // - go through vertex neighborhood and update all 'other' edges that point to old_vertex_i
                // so that they point to new_vertex_i
                struct HalfEdge* edge;
                int32_t edge_i = -1;
                int32_t i = 0;
                while( halfedgemesh_vertex_iterate(mesh, old_vertex_i, &edge, &edge_i, &i) ) {
                    if( edge->vertex == old_vertex_i ) {
                        edge->vertex = new_vertex_i;
                    }
                }

                mesh->vertices.array[new_vertex_i] = mesh->vertices.array[old_vertex_i];
                mesh->vertices.array[new_vertex_i].this = new_vertex_i;
            }

            iter_vertex++;
        }

        if( num_removed_faces && iter_face < (int32_t)mesh->faces.occupied ) {
            while( iter_removed_faces < num_removed_faces && removed_faces[iter_removed_faces] == iter_face ) {
                gap_faces++;
                iter_face++;
                iter_removed_faces++;
            }

            // - go through all edges of face and update face index to new_face_i
            if( gap_faces > 0 && iter_face < (int32_t)mesh->faces.occupied ) {
                int32_t old_face_i = iter_face;
                int32_t new_face_i = iter_face - gap_faces;

                struct HalfEdge* edge;
                int32_t edge_i = -1;
                int32_t i = 0;
                while( halfedgemesh_face_iterate(mesh, old_face_i, &edge, &edge_i, &i) ) {
                    edge->face = new_face_i;
                }

                mesh->faces.array[new_face_i] = mesh->faces.array[old_face_i];
                mesh->faces.array[new_face_i].this = new_face_i;
            }

            iter_face++;
        }

        if( num_removed_edges && iter_edge < (int32_t)mesh->edges.occupied ) {
            while( iter_removed_edges < num_removed_edges && removed_edges[iter_removed_edges] == iter_edge ) {
                gap_edges++;
                iter_edge++;
                iter_removed_edges++;
            }

            if( gap_edges > 0 && iter_edge < (int32_t)mesh->edges.occupied ) {
                log_assert( iter_edge >= gap_edges );

                int32_t old_edge_i = iter_edge;
                int32_t new_edge_i = iter_edge - gap_edges;

                struct HalfEdge* this = &mesh->edges.array[old_edge_i];

                // - pfft, I forgot to set the edges own index to new_edge_i first,
                // resulting in weird problems
                this->this = new_edge_i;

                // - I need to change the edges that point to this index, that means I have
                // to change this prevs edges next index, this next edges prev index and the other
                // edges other index
                mesh->edges.array[this->prev].next = new_edge_i;
                mesh->edges.array[this->next].prev = new_edge_i;
                mesh->edges.array[this->other].other = new_edge_i;

                // - I need to check the vertex of the prev edge if it has this edge as outgoing
                if( mesh->vertices.array[mesh->edges.array[this->prev].vertex].edge == old_edge_i ) {
                    mesh->vertices.array[mesh->edges.array[this->prev].vertex].edge = new_edge_i;
                }

                // - and I need to check the face if it has this edge as first edge
                if( mesh->faces.array[this->face].edge == old_edge_i ) {
                    mesh->faces.array[this->face].edge = new_edge_i;
                }

                mesh->edges.array[new_edge_i] = mesh->edges.array[old_edge_i];
            }

            iter_edge++;
        }
    }

    log_assert( num_removed_vertices >= 0 );
    log_assert( mesh->vertices.occupied >= (size_t)num_removed_vertices );
    mesh->vertices.occupied -= (size_t)num_removed_vertices;

    log_assert( num_removed_faces >= 0 );
    log_assert( mesh->faces.occupied >= (size_t)num_removed_faces );
    mesh->faces.occupied -= (size_t)num_removed_faces;

    log_assert( num_removed_edges >= 0 );
    log_assert( mesh->edges.occupied >= (size_t)num_removed_edges );
    mesh->edges.occupied -= (size_t)num_removed_edges;

    free(edge_check);
    free(face_has_normal);
    free(face_normals);
    free(removed_faces);
    free(removed_edges);
    free(removed_vertices);
    free(attached_edges);
}

void halfedgemesh_verify(const struct HalfEdgeMesh* mesh) {
    log_assert( mesh != NULL );
    log_assert( mesh->vertices.occupied < INT32_MAX );
    log_assert( mesh->edges.occupied < INT32_MAX );
    log_assert( mesh->faces.occupied < INT32_MAX );

    for( int32_t face_i = 0; face_i < (int32_t)mesh->faces.occupied; face_i++ ) {
        struct HalfEdgeFace* face = &mesh->faces.array[face_i];
        log_assert( face->edge > -1 );
        log_assert( face->this == face_i );

        struct HalfEdge* this = &mesh->edges.array[face->edge];
        int32_t i = 0;
        while( (i == 0 || this->this != face->edge) && i <= face->size*2 ) {
            log_assert( this->face == face_i );

            struct HalfEdge* other = &mesh->edges.array[this->other];
            log_assert( this->vertex != other->vertex );

            log_assert( this->prev != this->this );
            log_assert( this->next != this->this );
            log_assert( this->prev != this->next );

            log_assert( other->prev != other->this );
            log_assert( other->next != other->this );
            log_assert( other->prev != other->next );

            log_assert( other->prev != this->this );
            log_assert( other->next != this->this );
            log_assert( other->prev != this->next );

            log_assert( this->prev != other->this );
            log_assert( this->next != other->this );
            log_assert( this->prev != other->next );

            log_assert( this->other == this->this + 1 || this->other == this->this - 1 );

            // other prev vertex must be equal this vertex
            log_assert( mesh->edges.array[other->prev].vertex == this->vertex );
            // other vertex must be equal this prev vertex
            log_assert( other->vertex == mesh->edges.array[this->prev].vertex );

            struct HalfEdgeVertex* vertex = &mesh->vertices.array[this->vertex];
            log_assert( vertex->this == this->vertex );

            struct HalfEdge* iter_edge = &mesh->edges.array[vertex->edge];
            int32_t j = 0;
            while( j == 0 || iter_edge->this != vertex->edge ) {
                if( iter_edge->vertex == this->vertex ) {
                    log_assert( j % 2 == 1 );
                    iter_edge = &mesh->edges.array[iter_edge->next];
                } else {
                    log_assert( j % 2 == 0 );
                    iter_edge = &mesh->edges.array[iter_edge->other];
                }

                j++;
                log_assert( j < (int32_t)mesh->edges.occupied );
            }

            this = &mesh->edges.array[this->next];
            i++;
        }
        log_assert( i == face->size );
    }
}
