#include "geometry_draw.h"

void draw_solid_normals(struct Canvas* canvas,
                        int32_t layer,
                        const Mat model_matrix,
                        const Color color,
                        float line_thickness,
                        const struct Solid* solid,
                        float scale)
{
    float* vertices = solid->vertices;
    float* normals = solid->normals;

    for( uint32_t i = 0; i < solid->indices_size; i++ ) {
        uint32_t index = solid->indices[i];

        Vec4f normal = { normals[index*3+0], normals[index*3+1], normals[index*3+2], 1.0f };
        Vec4f vertex = { vertices[index*3+0], vertices[index*3+1], vertices[index*3+2], 1.0f };

        draw_vec(canvas, layer, model_matrix, color, line_thickness, normal, vertex, 0.0f, scale);
    }
}

void draw_halfedgemesh_wire(struct Canvas* canvas,
                            int32_t layer,
                            const Mat model_matrix,
                            const Color color,
                            float line_thickness,
                            const struct HalfEdgeMesh* mesh)
{
    uint32_t num_edges = mesh->edges.occupied;

    for( uint32_t edge_i = 0; edge_i < num_edges; edge_i += 2 ) {
        const struct HalfEdge* this_edge = &mesh->edges.array[edge_i];
        const struct HalfEdge* other_edge = &mesh->edges.array[this_edge->other];
        int32_t this_index = this_edge->vertex;
        int32_t other_index = other_edge->vertex;

        draw_line(canvas, layer, model_matrix, color, line_thickness, mesh->vertices.array[this_index].position, mesh->vertices.array[other_index].position);
    }
}

void draw_halfedgemesh_face(struct Canvas* canvas,
                            int32_t layer,
                            const Mat model_matrix,
                            const Color color,
                            float line_thickness,
                            const struct HalfEdgeMesh* mesh,
                            int32_t face_i)
{
    log_assert( mesh != NULL );
    log_assert( face_i < mesh->faces.occupied );

    const struct HalfEdgeFace* face = &mesh->faces.array[face_i];
    log_assert(face->size > 0);

    struct HalfEdge* edge = &mesh->edges.array[face->edge];
    do {
        struct HalfEdge* other = &mesh->edges.array[edge->other];
        draw_line(canvas, layer, model_matrix, color, line_thickness, mesh->vertices.array[edge->vertex].position, mesh->vertices.array[other->vertex].position);
        edge = &mesh->edges.array[edge->next];
    } while(edge->this != face->edge);
}

void draw_halfedgemesh_edge(struct Canvas* canvas,
                            int32_t layer,
                            const Mat model_matrix,
                            const Color color,
                            float line_thickness,
                            const struct HalfEdgeMesh* mesh,
                            int32_t edge_i)
{
    log_assert( mesh != NULL );
    log_assert( edge_i < mesh->edges.occupied );

    const struct HalfEdge* edge = &mesh->edges.array[edge_i];
    const struct HalfEdge* other = &mesh->edges.array[edge->other];

    draw_line(canvas, layer, model_matrix, color, line_thickness, mesh->vertices.array[edge->vertex].position, mesh->vertices.array[other->vertex].position);
}

void draw_halfedgemesh_vertex(struct Canvas* canvas,
                              int32_t layer,
                              const Mat model_matrix,
                              const Color color,
                              float line_thickness,
                              const struct HalfEdgeMesh* mesh,
                              int32_t vertex_i,
                              float scale)
{
    log_assert( mesh != NULL );
    log_assert( vertex_i < mesh->vertices.occupied );

    const struct HalfEdgeVertex* vertex = &mesh->vertices.array[vertex_i];
    const struct HalfEdge* edge = &mesh->edges.array[vertex->edge];
    do {
        const struct HalfEdge* other = &mesh->edges.array[edge->other];
        draw_vec(canvas, layer, model_matrix, color, line_thickness, other->normal, vertex->position, 0.0f, scale);
        edge = &mesh->edges.array[other->next];
    } while(edge->this != vertex->edge);
}
