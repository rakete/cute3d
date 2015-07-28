#include "geometry_halfedgemesh.h"

int halfedgemesh_create(struct HalfEdgeMesh* mesh) {
    mesh->size = 0;

    mesh->vertices.capacity = 0;
    mesh->vertices.reserved = 0;
    mesh->vertices.array = NULL;

    mesh->faces.capacity = 0;
    mesh->faces.reserved = 0;
    mesh->faces.array = NULL;

    mesh->edges.capacity = 0;
    mesh->edges.reserved = 0;
    mesh->edges.array = NULL;

    return 0;
}

int halfedgemesh_destroy(struct HalfEdgeMesh* mesh) {
    free(mesh->vertices.array);
    free(mesh->faces.array);
    free(mesh->edges.array);

    halfedgemesh_create(mesh);

    return 0;
}

unsigned int halfedgemesh_alloc_vertices(struct HalfEdgeMesh* mesh, unsigned int n) {
    int new_capacity = mesh->vertices.capacity + n;
    struct HalfEdgeVertex* new_array = realloc(mesh->vertices.array, new_capacity * sizeof(struct HalfEdgeVertex));

    if( new_array ) {
        mesh->vertices.array = new_array;
        mesh->vertices.capacity = new_capacity;
        return n * sizeof(struct HalfEdgeVertex);
    }

    return 0;
}

unsigned int halfedgemesh_alloc_faces(struct HalfEdgeMesh* mesh, unsigned int n) {
    int new_capacity = mesh->faces.capacity + n;
    struct HalfEdgeFace* new_array = realloc(mesh->faces.array, new_capacity * sizeof(struct HalfEdgeFace));

    if( new_array ) {
        mesh->faces.array = new_array;
        mesh->faces.capacity = new_capacity;
        return n * sizeof(struct HalfEdgeFace);
    }

    return 0;
}


unsigned int halfedgemesh_alloc_edges(struct HalfEdgeMesh* mesh, unsigned int n) {
    int new_capacity = mesh->edges.capacity + n;
    struct HalfEdge* new_array = realloc(mesh->edges.array, new_capacity * sizeof(struct HalfEdge));

    if( new_array ) {
        mesh->edges.array = new_array;
        mesh->edges.capacity = new_capacity;
        return n * sizeof(struct HalfEdge);
    }

    return 0;
}

void halfedgemesh_append(struct HalfEdgeMesh* mesh, const struct Solid* solid) {
    assert( solid->size > 0 );
    assert( solid->size < UINT_MAX/3 );
    assert( solid->vertices != NULL );
    assert( solid->elements != NULL );
    assert( solid->triangles != NULL );
    assert( solid->normals != NULL );

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
    // vertex_map[1,2,3] == UINT_MAX and since these are all unique they would be stored in
    // the mesh and that new mesh index would be written into vertex_map[1,2,3] = new_index,
    // then if we processed triangle 2 3 4, we'd know that only 4 would be unique and needs
    // to be stored, but 2 3 would already be in vertex_map and we could even find their
    // respective new indices where they are stored in the mesh
    //
    // we also use this loop to find the largest vertex index from solid->triangles, since
    // each unique vertex should be represented by one unique index, the largest index + 1
    // should be the number of unique vertices
    unsigned int unique_vertex_map[solid->size];
    unsigned int num_unique_vertices = 0;
    for( unsigned int i = 0; i < solid->size; i++ ) {
        unique_vertex_map[i] = UINT_MAX;

        if( solid->triangles[i] > num_unique_vertices || i == 0 ) {
            num_unique_vertices = solid->triangles[i];
        }
    }
    num_unique_vertices += 1;

    // the edges_map is initialized like the unique_vertex_map and used in a similar way, to be used
    // later when we need to decide if an edge has already been seen so that we can set the
    // correct other indices
    unsigned int num_triangles = solid->size/3;
    unsigned int edges_map[num_unique_vertices][num_unique_vertices];
    for( unsigned int i = 0; i < num_triangles; i++ ) {
        unsigned int a = solid->triangles[i*3+0];
        unsigned int b = solid->triangles[i*3+1];
        unsigned int c = solid->triangles[i*3+2];

        edges_map[a][b] = UINT_MAX;
        edges_map[a][c] = UINT_MAX;

        edges_map[b][a] = UINT_MAX;
        edges_map[b][c] = UINT_MAX;

        edges_map[c][a] = UINT_MAX;
        edges_map[c][b] = UINT_MAX;
    }

    // we need to make sure that we'll have enough space reserved in the mesh for all
    // the data that we are going to add
    // this ist lacking error handling
    unsigned int free_vertices_capacity = mesh->vertices.capacity - mesh->vertices.reserved;
    if( free_vertices_capacity < num_unique_vertices ) {
        halfedgemesh_alloc_vertices(mesh, num_unique_vertices - free_vertices_capacity);
    }

    unsigned int free_faces_capacity = mesh->faces.capacity - mesh->faces.reserved;
    if( free_faces_capacity < num_triangles ) {
        halfedgemesh_alloc_faces(mesh, num_triangles - free_faces_capacity);
    }

    unsigned int free_edges_capacity = mesh->edges.capacity - mesh->edges.reserved;
    if( free_edges_capacity < num_triangles*3 ) {
        halfedgemesh_alloc_edges(mesh, num_triangles*3 - free_edges_capacity);
    }

    unsigned int vertex_i = mesh->vertices.reserved;
    unsigned int face_i = mesh->faces.reserved;
    unsigned int edge_i = mesh->edges.reserved;
    for( unsigned int i = 0; i < num_triangles; i++ ) {

        // mainloop iterates once over all triangles, then iterates over the vertices of a triangle,
        // collects each vertex normal, checks if the vertex is unique by looking up its triangle index in
        // unique_vertex_map, and if it is unique it adds it to the mesh and increases vertex_i, the
        // counter for vertices added to the mesh
        float normals[9] = { 0 };
        unsigned int triangle[3] = {
            solid->triangles[i*3+0],
            solid->triangles[i*3+1],
            solid->triangles[i*3+2]
        };

        for( unsigned int j = 0; j < 3; j++ ) {
            unsigned int solid_i = i*3*3+j*3;
            normals[j*3+0] = solid->normals[solid_i+0];
            normals[j*3+1] = solid->normals[solid_i+1];
            normals[j*3+2] = solid->normals[solid_i+2];

            if( unique_vertex_map[triangle[j]] == UINT_MAX ) {
                unique_vertex_map[triangle[j]] = vertex_i;

                mesh->vertices.array[vertex_i].position[0] = solid->vertices[solid_i+0];
                mesh->vertices.array[vertex_i].position[1] = solid->vertices[solid_i+1];
                mesh->vertices.array[vertex_i].position[2] = solid->vertices[solid_i+2];
                mesh->vertices.array[vertex_i].position[3] = 1.0;

                mesh->vertices.array[vertex_i].edge = UINT_MAX;

                vertex_i += 1;
            }
        }

        // convience names for the three vertex indices from triangle that we'll use to represent the halfedges
        // a -> b, b -> c and c -> a
        unsigned int a = triangle[0];
        unsigned int b = triangle[1];
        unsigned int c = triangle[2];
        assert( a != b && b != c && c != a );

        // we assume each pair of vertex indices, representing an edge, gets seen once as a -> b and once again
        // as b -> a (which conveniently fits our model of representing each edge as two half edges pointing
        // into opposite directions)
        //
        // so we assert that halfedge index for the current halfedge has never been set in edges_map
        // and should still be UINT_MAX
        assert( edges_map[a][b] == UINT_MAX );
        assert( edges_map[b][c] == UINT_MAX );
        assert( edges_map[c][a] == UINT_MAX );

        // indices for the three new halfedges we'll create in the mesh
        unsigned int ca_i = UINT_MAX;
        unsigned int ab_i = UINT_MAX;
        unsigned int bc_i = UINT_MAX;

        // if we had already seen the current edge in a previous iteration, then with flipped indices (in this
        // a -> b case we would have seen b -> a previously), and we can check that by testing if edges_map[b][a]
        // has already been set to something other then UINT_MAX
        unsigned int ab_other_i = UINT_MAX;
        if( edges_map[b][a] != UINT_MAX ) {
            ab_other_i = edges_map[b][a];
            ab_i = ab_other_i + 1;
            mesh->edges.array[ab_other_i].other = ab_i;
        }

        unsigned int bc_other_i = UINT_MAX;
        if( edges_map[c][b] != UINT_MAX ) {
            bc_other_i = edges_map[c][b];
            bc_i = bc_other_i + 1;
            mesh->edges.array[bc_other_i].other = bc_i;
        }

        unsigned int ca_other_i = UINT_MAX;
        if( edges_map[a][c] != UINT_MAX ) {
            ca_other_i = edges_map[a][c];
            ca_i = ca_other_i + 1;
            mesh->edges.array[ca_other_i].other = ca_i;
        }

        unsigned int edge_i_inc = 0;
        if( ca_i == UINT_MAX ) {
            ca_i = edge_i + edge_i_inc;
            edge_i_inc += 2;
        }

        if( ab_i == UINT_MAX ) {
            ab_i = edge_i + edge_i_inc;
            edge_i_inc += 2;
        }

        if( bc_i == UINT_MAX ) {
            bc_i = edge_i + edge_i_inc;
            edge_i_inc += 2;
        }

        assert( ab_i != UINT_MAX );
        assert( bc_i != UINT_MAX );
        assert( ca_i != UINT_MAX );

        // this could be in an else clause, but we'll just do it everytime even when it should be uneccessary half
        // the time
        // we need to set this halfedges index in the edges_map so we can use it as other index when we'll see the
        // current edge again in the future
        edges_map[a][b] = ab_i;

        // finally construct the halfedge for a -> b
        struct HalfEdge* ab_ptr = &mesh->edges.array[ab_i];
        *ab_ptr = (struct HalfEdge){
            .normal[0] = normals[3],
            .normal[1] = normals[4],
            .normal[2] = normals[5],
            .vertex = unique_vertex_map[b],
            .face = face_i,
            .next = bc_i,
            .this = ab_i,
            .prev = ca_i,
            .other = ab_other_i
        };

        if( mesh->vertices.array[unique_vertex_map[a]].edge ) {
            mesh->vertices.array[unique_vertex_map[a]].edge = ab_i;
        }

        edges_map[b][c] = bc_i;

        struct HalfEdge* bc_ptr = &mesh->edges.array[bc_i];
        *bc_ptr = (struct HalfEdge){
            .normal[0] = normals[6],
            .normal[1] = normals[7],
            .normal[2] = normals[8],
            .vertex = unique_vertex_map[c],
            .face = face_i,
            .next = ca_i,
            .this = bc_i,
            .prev = ab_i,
            .other = bc_other_i
        };

        if( mesh->vertices.array[unique_vertex_map[b]].edge ) {
            mesh->vertices.array[unique_vertex_map[b]].edge = bc_i;
        }

        edges_map[c][a] = ca_i;

        struct HalfEdge* ca_ptr = &mesh->edges.array[ca_i];
        *ca_ptr = (struct HalfEdge){
            .normal[0] = normals[0],
            .normal[1] = normals[1],
            .normal[2] = normals[2],
            .vertex = unique_vertex_map[a],
            .face = face_i,
            .next = ab_i,
            .this = ca_i,
            .prev = bc_i,
            .other = ca_other_i
        };

        if( mesh->vertices.array[unique_vertex_map[c]].edge ) {
            mesh->vertices.array[unique_vertex_map[c]].edge = ca_i;
        }

        // after creating the halfedges the face gets set, this just as simple as it looks since the face
        // contains nothing but an index to any of its halfedges
        struct HalfEdgeFace* face_ptr = &mesh->faces.array[face_i];
        face_ptr->size = 3;
        face_ptr->edge = ca_i;

        // we need to keep track about how many faces and edges we added to the mesh with
        // these index counters that indicate the position where we'll add the next face/edge
        // when the iteration continues to the next triangle
        face_i += 1;
        edge_i += edge_i_inc;

        mesh->size += 3;

        assert(vertex_i <= mesh->vertices.capacity);
        mesh->vertices.reserved = vertex_i;

        assert(face_i <= mesh->faces.capacity);
        mesh->faces.reserved = face_i;

        assert(edge_i <= mesh->edges.capacity);
        mesh->edges.reserved = edge_i;
    }
}

void solid_triangulate(float* vertices, unsigned int n, unsigned int* triangles, unsigned int m) {
    assert( n >= 3 );
    assert( n < UINT_MAX/3 );
    assert( m <= 3 * n - 2);

    if( n == 3 || n == 4 ) {
        triangles[0] = 0;
        triangles[1] = 1;
        triangles[2] = 2;
    }

    // #YOLO
    if( n == 4 ) {
        triangles[3] = 2;
        triangles[4] = 3;
        triangles[5] = 0;
    }
}

void halfedgemesh_flush(const struct HalfEdgeMesh* mesh, struct Solid* solid) {
    assert(mesh->size == solid->size);

    unsigned int vertices_offset = 0;
    unsigned int normals_offset = 0;
    unsigned int elements_offset = 0;
    unsigned int triangles_offset = 0;

    /* for( unsigned int i = 0; i < mesh->size*3; i++ ) { */
    /*     solid->vertices[i] = 0.0; */
    /*     solid->normals[i] = 0.0; */
    /*     if( i < mesh->size ) { */
    /*         solid->elements[i] = 0; */
    /*         solid->triangles[i] = 0; */
    /*     } */
    /* } */

    for( unsigned int i = 0; i < mesh->faces.reserved; i++ ) {
        struct HalfEdgeFace* face = &mesh->faces.array[i];

        if( face->edge == UINT_MAX ) {
            continue;
        }

        float face_vertices[face->size*3];
        float face_normals[face->size*3];
        unsigned int face_triangles[face->size];

        /* for( unsigned int j = 0; j < face->size*3; j++ ) { */
        /*     face_vertices[j] = 0.0; */
        /*     face_normals[j] = 0.0; */
        /*     if( j < face->size ) { */
        /*         face_triangles[j] = 0; */
        /*     } */
        /* } */

        struct HalfEdge* current_edge = &mesh->edges.array[face->edge];
        for( unsigned int face_vertex_i = 0; face_vertex_i < face->size; face_vertex_i++ ) {
            face_vertices[face_vertex_i*3+0] = mesh->vertices.array[current_edge->vertex].position[0];
            face_vertices[face_vertex_i*3+1] = mesh->vertices.array[current_edge->vertex].position[1];
            face_vertices[face_vertex_i*3+2] = mesh->vertices.array[current_edge->vertex].position[2];

            face_normals[face_vertex_i*3+0] = current_edge->normal[0];
            face_normals[face_vertex_i*3+1] = current_edge->normal[1];
            face_normals[face_vertex_i*3+2] = current_edge->normal[2];

            face_triangles[face_vertex_i] = current_edge->vertex;

            current_edge = &mesh->edges.array[current_edge->next];
        }

        unsigned int tesselation_size = 3 * (face->size - 2);
        unsigned int face_tesselation[tesselation_size];
        solid_triangulate(face_vertices, face->size, face_tesselation, tesselation_size);

        for( unsigned int j = 0; j < tesselation_size ; j++ ) {
            unsigned int k = face_tesselation[j];
            unsigned int x = k*3+0;
            unsigned int y = k*3+1;
            unsigned int z = k*3+2;

            solid->vertices[vertices_offset+0] = face_vertices[x];
            solid->vertices[vertices_offset+1] = face_vertices[y];
            solid->vertices[vertices_offset+2] = face_vertices[z];
            vertices_offset += 3;

            solid->normals[normals_offset+0] = face_normals[x];
            solid->normals[normals_offset+1] = face_normals[y];
            solid->normals[normals_offset+2] = face_normals[z];
            normals_offset += 3;

            solid->elements[elements_offset] = elements_offset;
            elements_offset += 1;

            solid->triangles[triangles_offset] = face_triangles[k];
            triangles_offset += 1;
        }
    }
}

int halfedgemesh_face_normal(struct HalfEdgeMesh* mesh, unsigned int face_i, Vec3f equal_normal, Vec3f average_normal) {
    assert(face_i <= mesh->faces.reserved);
    assert(equal_normal != NULL || average_normal != NULL);

    struct HalfEdgeFace* face = &mesh->faces.array[face_i];

    struct HalfEdge* first_edge = &mesh->edges.array[face->edge];
    struct HalfEdge* second_edge = first_edge;

    int result = 0;
    if( equal_normal != NULL ) {
        vec_copy3f(first_edge->normal, equal_normal);
    }

    if( average_normal != NULL ) {
        average_normal[0] = first_edge->normal[0] / face->size;
        average_normal[1] = first_edge->normal[1] / face->size;
        average_normal[2] = first_edge->normal[2] / face->size;
        result = 1;
    }

    for( unsigned int i = 0; i < face->size-1; i++ ) {
        second_edge = &mesh->edges.array[first_edge->next];
        if( equal_normal != NULL && vequal3f(first_edge->normal, second_edge->normal) ) {
            result = 1;
        }

        if( average_normal != NULL ) {
            average_normal[0] += second_edge->normal[0] / face->size;
            average_normal[1] += second_edge->normal[1] / face->size;
            average_normal[2] += second_edge->normal[2] / face->size;
        } else if( result == 0 ) {
            break;
        }
    }

    return result;
}

int halfedgemesh_face_iterate(struct HalfEdgeMesh* mesh, unsigned int face_i, struct HalfEdge** edge, unsigned int* edge_i, unsigned int* i) {
    assert( face_i <= mesh->faces.reserved );
    assert( edge != NULL );
    assert( i != NULL );
    assert( edge_i != NULL );

    struct HalfEdgeFace* face = &mesh->faces.array[face_i];

    if( *i == 0 || *edge_i == UINT_MAX ) {
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

int halfedgemesh_vertex_iterate(struct HalfEdgeMesh* mesh, unsigned int vertex_i, struct HalfEdge** edge, unsigned int* edge_i, unsigned int* i) {
    assert( vertex_i <= mesh->vertices.reserved );
    assert( edge != NULL );
    assert( edge_i != NULL );
    assert( i != NULL );

    int result = 0;
    if( *i == 0 || *edge_i == UINT_MAX ) {
        *edge_i = mesh->vertices.array[vertex_i].edge;
        result = 1;
    } else if( *i % 2 == 1 ) {
        *edge_i = mesh->edges.array[*edge_i].other;
        assert( mesh->edges.array[*edge_i].vertex == vertex_i );
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

void halfedgemesh_compress(struct HalfEdgeMesh* mesh) {
    // for all edges (_both_ halfedges), check if the face normals
    // on both sides are equal, if they are, mark this edge for removal
    //
    // after checking all edges, remove those edges marked for removal,
    // remove all faces marked for removal
    int edge_check[mesh->edges.reserved];
    for( unsigned int i = 0; i < mesh->edges.reserved; i++ ) {
        edge_check[i] = 1;
    }

    int face_has_normal[mesh->size/3];
    float face_normals[mesh->size];
    for( unsigned int i = 0; i < mesh->size; i++ ) {
        if( i < mesh->size/3 ) {
            face_has_normal[i] = 0;
        }
        face_normals[i] = 0.0f;
    }

    unsigned int removed_faces[mesh->faces.reserved];
    unsigned int num_removed_faces = 0;

    unsigned int removed_edges[mesh->edges.reserved];
    unsigned int num_removed_edges = 0;

    unsigned int removed_vertices[mesh->vertices.reserved];
    unsigned int num_removed_vertices = 0;

    unsigned int attached_edges[mesh->vertices.reserved];
    for( unsigned int i = 0; i < mesh->vertices.reserved; i++ ) {
        attached_edges[i] = 0;
    }

    for( unsigned int this_i = 0; this_i < mesh->edges.reserved; this_i++ ) {
        if( edge_check[this_i] > 0 ) {
            unsigned int other_i = mesh->edges.array[this_i].other;
            edge_check[this_i] = 0;
            edge_check[other_i] = 0;

            unsigned int face_one_i = mesh->edges.array[this_i].face;
            unsigned int face_two_i = mesh->edges.array[other_i].face;

            if( ! face_has_normal[face_one_i] ) {
                face_has_normal[face_one_i] = halfedgemesh_face_normal(mesh, face_one_i, face_normals+face_one_i*3, NULL);
            }

            if( ! face_has_normal[face_two_i] ) {
                face_has_normal[face_two_i] = halfedgemesh_face_normal(mesh, face_two_i, face_normals+face_two_i*3, NULL);
            }

            struct HalfEdge* this = &mesh->edges.array[this_i];
            struct HalfEdge* other = &mesh->edges.array[other_i];
            attached_edges[this->vertex] += 1;
            attached_edges[other->vertex] += 1;

            if( face_has_normal[face_one_i] && face_has_normal[face_two_i] ) {
                Vec3f normal_a = { 0, 0, 0 };
                normal_a[0] = face_normals[face_one_i*3+0];
                normal_a[1] = face_normals[face_one_i*3+1];
                normal_a[2] = face_normals[face_one_i*3+2];

                Vec3f normal_b = { 0, 0, 0 };
                normal_b[0] = face_normals[face_two_i*3+0];
                normal_b[1] = face_normals[face_two_i*3+1];
                normal_b[2] = face_normals[face_two_i*3+2];

                if( vequal3f3f(normal_a, normal_b) ) {
                    // a\  /b
                    //   +i
                    // 1 |e 2
                    //   +j
                    // c/  \d
                    assert( this->prev != this->this );
                    assert( this->next != this->this );
                    assert( this->prev != this->next );

                    assert( other->prev != other->this );
                    assert( other->next != other->this );
                    assert( other->prev != other->next );

                    assert( other->prev != this->this );
                    assert( other->next != this->this );
                    assert( other->prev != this->next );

                    assert( this->prev != other->this );
                    assert( this->next != other->this );
                    assert( this->prev != other->next );

                    unsigned int halfedge_a_i = this->next;
                    struct HalfEdge* halfedge_a = &mesh->edges.array[halfedge_a_i];

                    unsigned int halfedge_b_i = other->prev;
                    struct HalfEdge* halfedge_b = &mesh->edges.array[halfedge_b_i];

                    unsigned int halfedge_c_i = this->prev;
                    struct HalfEdge* halfedge_c = &mesh->edges.array[halfedge_c_i];

                    unsigned int halfedge_d_i = other->next;
                    struct HalfEdge* halfedge_d = &mesh->edges.array[halfedge_d_i];

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
                    unsigned int iter_edge_i = UINT_MAX;
                    unsigned int iter_i = 0;
                    while( halfedgemesh_face_iterate(mesh, face_two_i, &iter_edge, &iter_edge_i, &iter_i) ) {
                        iter_edge->face = face_one_i;
                    }

                    // increase face size of face one by face size of face size, - 2 because we are deleting an
                    // edge (two halfedges, one from each face)
                    assert( mesh->faces.array[face_one_i].size > 2 && mesh->faces.array[face_two_i].size > 2 );
                    mesh->faces.array[face_one_i].size += mesh->faces.array[face_two_i].size - 2;
                    // mark face 2 for removal
                    mesh->faces.array[face_two_i].edge = UINT_MAX;
                    removed_faces[num_removed_faces] = face_two_i;
                    num_removed_faces += 1;

                    // - if this halfedge is the face edge of face_one, change the face edge to the next
                    // edge
                    if( mesh->faces.array[face_one_i].edge == this->this ) {
                        mesh->faces.array[face_one_i].edge = this->next;
                    }

                    // - update a's next to b, and b's prev to a, and c's next to d, and d's prev to c
                    assert( halfedge_a->prev == this->this );
                    assert( halfedge_b->next == other->this );
                    assert( halfedge_c->next == this->this );
                    assert( halfedge_d->prev == other->this );

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

                    this->face = UINT_MAX;
                    other->face = UINT_MAX;
                    removed_edges[num_removed_edges] = this_i;
                    num_removed_edges += 1;
                    removed_edges[num_removed_edges] = other_i;
                    num_removed_edges += 1;

                    assert( halfedge_a->prev != halfedge_a->this );
                    assert( halfedge_a->next != halfedge_a->this );
                    assert( halfedge_a->prev != halfedge_a->next );

                    assert( halfedge_b->prev != halfedge_b->this );
                    assert( halfedge_b->next != halfedge_b->this );
                    assert( halfedge_b->prev != halfedge_b->next );

                    assert( halfedge_c->prev != halfedge_c->this );
                    assert( halfedge_c->next != halfedge_c->this );
                    assert( halfedge_c->prev != halfedge_c->next );

                    assert( halfedge_d->prev != halfedge_d->this );
                    assert( halfedge_d->next != halfedge_d->this );
                    assert( halfedge_d->prev != halfedge_d->next );

                    assert( this->prev != this->this );
                    assert( this->next != this->this );
                    assert( this->prev != this->next );

                    assert( other->prev != other->this );
                    assert( other->next != other->this );
                    assert( other->prev != other->next );

                    assert( other->prev != this->this );
                    assert( other->next != this->this );
                    assert( other->prev != this->next );

                    assert( this->prev != other->this );
                    assert( this->next != other->this );
                    assert( this->prev != other->next );
                }
            }
        }
    }

    for( unsigned int i = 0; i < mesh->vertices.reserved; i++ ) {
        //printf("attached %u %u\n", i, attached_edges[i]);
        if( attached_edges[i] <= 0 ) {
            mesh->vertices.array[i].edge = UINT_MAX;
            removed_vertices[num_removed_vertices] = i;
            num_removed_vertices++;
        }
    }

    // after putting everthing that needs to be deleted into removed_vertices, removed_edges and removed_faces,
    // I need to actually delete those from the arrays in struct HalfEdgeMesh* mesh
    // to do that I need to go through all arrays, and fill all gaps left by deleted primitives
    // with the primitives after, that still exist
    unsigned int iter_vertex = 0;
    unsigned int iter_face = 0;
    unsigned int iter_edge = 0;

    unsigned int iter_removed_vertices = 0;
    unsigned int iter_removed_faces = 0;
    unsigned int iter_removed_edges = 0;

    unsigned int gap_vertices = 0;
    unsigned int gap_faces = 0;
    unsigned int gap_edges = 0;

    // this loop is bad and I don't like it, but it seems to work so it will stay like this anyways
    while( (num_removed_vertices && iter_vertex < mesh->vertices.reserved) ||
           (num_removed_faces && iter_face < mesh->faces.reserved) ||
           (num_removed_edges && iter_edge < mesh->edges.reserved) )
    {
        // deleting vertices is untested
        if( num_removed_vertices && iter_vertex < mesh->vertices.reserved ) {
            while( iter_removed_vertices < num_removed_vertices && removed_vertices[iter_removed_vertices] == iter_vertex ) {
                gap_vertices++;
                iter_vertex++;
                iter_removed_vertices++;
            }

            if( gap_vertices > 0 && iter_vertex < mesh->vertices.reserved ) {
                unsigned int old_vertex_i = iter_vertex;
                unsigned int new_vertex_i = iter_vertex - gap_vertices;

                // - go through vertex neighborhood and update all 'other' edges that point to old_vertex_i
                // so that they point to new_vertex_i
                struct HalfEdge* edge;
                unsigned int edge_i = UINT_MAX;
                unsigned int i = 0;
                while( halfedgemesh_vertex_iterate(mesh, old_vertex_i, &edge, &edge_i, &i) ) {
                    if( edge->vertex == old_vertex_i ) {
                        edge->vertex = new_vertex_i;
                    }
                }

                mesh->vertices.array[new_vertex_i] = mesh->vertices.array[old_vertex_i];
            }

            iter_vertex++;
        }

        if( num_removed_faces && iter_face < mesh->faces.reserved ) {
            while( iter_removed_faces < num_removed_faces && removed_faces[iter_removed_faces] == iter_face ) {
                gap_faces++;
                iter_face++;
                iter_removed_faces++;
            }

            // - go through all edges of face and update face index to new_face_i
            if( gap_faces > 0 && iter_face < mesh->faces.reserved ) {
                unsigned int old_face_i = iter_face;
                unsigned int new_face_i = iter_face - gap_faces;

                struct HalfEdge* edge;
                unsigned int edge_i = UINT_MAX;
                unsigned int i = 0;
                while( halfedgemesh_face_iterate(mesh, old_face_i, &edge, &edge_i, &i) ) {
                    edge->face = new_face_i;
                }

                mesh->faces.array[new_face_i] = mesh->faces.array[old_face_i];
            }

            iter_face++;
        }

        if( num_removed_edges && iter_edge < mesh->edges.reserved ) {
            while( iter_removed_edges < num_removed_edges && removed_edges[iter_removed_edges] == iter_edge ) {
                gap_edges++;
                iter_edge++;
                iter_removed_edges++;
            }

            if( gap_edges > 0 && iter_edge < mesh->edges.reserved ) {
                //printf("iter_edge >= gap_edges: %u %u %u\n", iter_edge, gap_edges, iter_edge - gap_edges);
                assert( iter_edge >= gap_edges );

                unsigned old_edge_i = iter_edge;
                unsigned new_edge_i = iter_edge - gap_edges;

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

    mesh->vertices.reserved -= num_removed_vertices;
    mesh->faces.reserved -= num_removed_faces;
    mesh->edges.reserved -= num_removed_edges;
}

void halfedgemesh_verify(struct HalfEdgeMesh* mesh) {
    for( unsigned int face_i = 0; face_i < mesh->faces.reserved; face_i++ ) {
        struct HalfEdgeFace* face = &mesh->faces.array[face_i];
        if( face->edge == UINT_MAX ) {
            continue;
        }

        unsigned int seen_vertices[mesh->vertices.reserved];
        for( unsigned int i = 0; i < mesh->vertices.reserved; i++ ) {
            seen_vertices[i] = 0;
        }

        struct HalfEdge* this = &mesh->edges.array[face->edge];
        unsigned int i = 0;
        while( (i == 0 || this->this != face->edge) && i <= face->size*2 ) {
            //printf("this->face, face_i: %u %u\n", this->face, face_i);
            assert( this->face == face_i );

            seen_vertices[this->vertex] += 1;
            assert( seen_vertices[this->vertex] == 1 );

            struct HalfEdge* other = &mesh->edges.array[this->other];
            assert( this->vertex != other->vertex );

            assert( this->prev != this->this );
            assert( this->next != this->this );
            //printf("this->prev != this->next: %u %u\n", this->prev, this->next);
            assert( this->prev != this->next );

            assert( other->prev != other->this );
            assert( other->next != other->this );
            //printf("other->prev != other->next: %u %u\n", other->prev, other->next);
            assert( other->prev != other->next );

            //printf("other->prev != this->this: %u %u\n", other->prev, this->this);
            assert( other->prev != this->this );
            //printf("other->next != this->this: %u %u\n", other->next, this->this);
            assert( other->next != this->this );
            assert( other->prev != this->next );

            //printf("this->prev != other->this: %u %u\n", this->prev, other->this);
            assert( this->prev != other->this );
            //printf("this->next != other->this: %u %u\n", this->next, other->this);
            assert( this->next != other->this );
            assert( this->prev != other->next );

            assert( this->other == this->this + 1 || this->other == this->this - 1 );

            // other prev vertex must be equal this vertex
            assert( mesh->edges.array[other->prev].vertex == this->vertex );
            // other vertex must be equal this prev vertex
            assert( other->vertex == mesh->edges.array[this->prev].vertex );

            struct HalfEdgeVertex* vertex = &mesh->vertices.array[this->vertex];
            struct HalfEdge* iter_edge = &mesh->edges.array[vertex->edge];
            unsigned int j = 0;
            while( j == 0 || iter_edge->this != vertex->edge ) {
                if( iter_edge->vertex == this->vertex ) {
                    assert( j % 2 == 1 );
                    iter_edge = &mesh->edges.array[iter_edge->next];
                } else {
                    assert( j % 2 == 0 );
                    iter_edge = &mesh->edges.array[iter_edge->other];
                }

                j++;
                assert( j < mesh->edges.reserved );
            }

            this = &mesh->edges.array[this->next];
            i++;
        }
        assert( i == face->size );
    }
}
