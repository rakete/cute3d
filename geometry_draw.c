#include "geometry_draw.h"

void draw_solid_normals(struct Canvas* canvas,
                        int32_t layer,
                        const Mat model_matrix,
                        const Color color,
                        const struct Solid* solid,
                        float scale)
{
    float* vertices = solid->vertices;
    float* normals = solid->normals;

    for( uint32_t i = 0; i < solid->indices_size; i++ ) {
        uint32_t index = solid->indices[i];

        Vec4f normal = { normals[index*3+0], normals[index*3+1], normals[index*3+2], 1.0f };
        Vec4f vertex = { vertices[index*3+0], vertices[index*3+1], vertices[index*3+2], 1.0 };

        draw_vec(canvas, layer, model_matrix, color, normal, vertex, 0.0f, scale);
    }
}

void draw_halfedgemesh_wire(struct Canvas* canvas,
                            int32_t layer,
                            const Mat model_matrix,
                            const Color color,
                            const struct HalfEdgeMesh* mesh)
{
    uint32_t num_attributes = mesh->vertices.occupied;
    uint32_t num_edges = mesh->edges.occupied;

    float vertices[num_attributes*3];
    uint8_t colors[num_attributes*4];
    uint32_t lines[num_edges];

    bool processed_vertices[num_attributes];
    memset(processed_vertices, false, sizeof(bool) * num_attributes);

    int32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;
    for( uint32_t edge_i = 0; edge_i < num_edges; edge_i += 2 ) {
        const struct HalfEdge* this_edge = &mesh->edges.array[edge_i];
        const struct HalfEdge* other_edge = &mesh->edges.array[this_edge->other];

        if( ! processed_vertices[this_edge->vertex] ) {
            int32_t this_index = this_edge->vertex;
            mat_mul_vec3f(model_matrix, mesh->vertices.array[this_index].position, &vertices[this_index*3]);
            color_copy(color, &colors[this_index*4]);

            log_assert( offset + this_index >= 0 );
            lines[edge_i+0] = (uint32_t)(offset + this_index);
        }

        if( ! processed_vertices[other_edge->vertex] ) {
            int32_t other_index = other_edge->vertex;
            mat_mul_vec3f(model_matrix, mesh->vertices.array[other_index].position, &vertices[other_index*3]);
            color_copy(color, &colors[other_index*4]);

            log_assert( offset + other_index >= 0 );
            lines[edge_i+1] = (uint32_t)(offset + other_index);
        }
    }

    draw_add_shader(canvas, gl_lines, "gl_lines_shader");

    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, num_attributes, vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE, num_attributes, colors);
    canvas_append_indices(canvas, layer, CANVAS_PROJECT_WORLD, "gl_lines_shader", GL_LINES, num_edges, lines, 0);
}

void draw_halfedgemesh_face(struct Canvas* canvas,
                            int32_t layer,
                            const Mat model_matrix,
                            const Color color,
                            const struct HalfEdgeMesh* mesh,
                            uint32_t face_i)
{
    log_assert( mesh != NULL );
    log_assert( face_i < mesh->faces.occupied );

    const struct HalfEdgeFace* face = &mesh->faces.array[face_i];
    log_assert(face->size > 0);

    float vertices[face->size*3];
    uint8_t colors[face->size*4];
    uint32_t lines[face->size*2];

    uint32_t i = 0;
    struct HalfEdge* edge = &mesh->edges.array[face->edge];
    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;
    do {
        mat_mul_vec3f(model_matrix, mesh->vertices.array[edge->vertex].position, &vertices[i*3]);
        lines[i*2+0] = offset + i+0;
        lines[i*2+1] = offset + i+1;
        edge = &mesh->edges.array[edge->next];

        color_copy(color, &colors[i*4]);

        i += 1;
    } while(edge->this != face->edge);
    lines[i*2-1] = offset;

    draw_add_shader(canvas, gl_lines, "gl_lines_shader");

    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, (size_t)face->size, vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE, (size_t)face->size, colors);
    canvas_append_indices(canvas, layer, CANVAS_PROJECT_WORLD, "gl_lines_shader", GL_LINES, (size_t)face->size*2, lines, 0);
}

void draw_halfedgemesh_edge(struct Canvas* canvas,
                            int32_t layer,
                            const Mat model_matrix,
                            const Color color,
                            const struct HalfEdgeMesh* mesh,
                            uint32_t edge_i)
{
    log_assert( mesh != NULL );
    log_assert( edge_i < mesh->edges.occupied );

    const struct HalfEdge* edge = &mesh->edges.array[edge_i];
    const struct HalfEdge* other = &mesh->edges.array[edge->other];

    float vertices[2*3];
    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;
    uint32_t line[2] = {offset,offset+1};

    mat_mul_vec3f(model_matrix, mesh->vertices.array[edge->vertex].position, &vertices[0]);
    mat_mul_vec3f(model_matrix, mesh->vertices.array[other->vertex].position, &vertices[3]);

    uint8_t colors[2*4] =
        { color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3] };

    draw_add_shader(canvas, gl_lines, "gl_lines_shader");

    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, 2, vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE, 2, colors);
    canvas_append_indices(canvas, layer, CANVAS_PROJECT_WORLD, "gl_lines_shader", GL_LINES, 2, line, 0);
}

void draw_halfedgemesh_vertex(struct Canvas* canvas,
                              int32_t layer,
                              const Mat model_matrix,
                              const Color color,
                              const struct HalfEdgeMesh* mesh,
                              uint32_t vertex_i,
                              float scale)
{
    log_assert( mesh != NULL );
    log_assert( vertex_i < mesh->vertices.occupied );

    const struct HalfEdgeVertex* vertex = &mesh->vertices.array[vertex_i];
    const struct HalfEdge* edge = &mesh->edges.array[vertex->edge];
    do {
        const struct HalfEdge* other = &mesh->edges.array[edge->other];
        draw_vec(canvas, layer, model_matrix, color, other->normal, vertex->position, 0.0f, scale);
        edge = &mesh->edges.array[other->next];
    } while(edge->this != vertex->edge);
}
