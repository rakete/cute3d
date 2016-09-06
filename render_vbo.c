/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "render_vbo.h"

void vbo_mesh_create_from_solid(const struct Solid* solid, struct Vbo* const vbo, struct Ibo* const ibo, struct VboMesh* mesh) {
    log_assert( solid != NULL );
    log_assert( mesh != NULL );

    vbo_mesh_create(vbo, ibo, mesh);

    size_t vertices_n = vbo_mesh_append_attributes(mesh, SHADER_ATTRIBUTE_VERTEX, VERTEX_SIZE, GL_FLOAT, solid->size, solid->vertices);
    log_assert( vertices_n == solid->size, "%zu == %zu\n", vertices_n, solid->size );

    if( vbo->buffer[SHADER_ATTRIBUTE_VERTEX_NORMAL].id ) {
        size_t normals_n = vbo_mesh_append_attributes(mesh, SHADER_ATTRIBUTE_VERTEX_NORMAL, NORMAL_SIZE, GL_FLOAT, solid->size, solid->normals);
        log_assert( normals_n == solid->size );
    }

    if( vbo->buffer[SHADER_ATTRIBUTE_DIFFUSE_COLOR].id ) {
        size_t colors_n = vbo_mesh_append_attributes(mesh, SHADER_ATTRIBUTE_DIFFUSE_COLOR, COLOR_SIZE, GL_UNSIGNED_BYTE, solid->size, solid->colors);
        log_assert( colors_n == solid->size );
    }

    if( vbo->buffer[SHADER_ATTRIBUTE_VERTEX_TEXCOORD].id ) {
        size_t texcoords_n = vbo_mesh_append_attributes(mesh, SHADER_ATTRIBUTE_VERTEX_TEXCOORD, TEXCOORD_SIZE, GL_FLOAT, solid->size, solid->texcoords);
        log_assert( texcoords_n == solid->size );
    }

    if( vbo->buffer[SHADER_ATTRIBUTE_SMOOTH_NORMAL].id ) {
        float* smooth_normals = (float*)malloc(solid->size * NORMAL_SIZE * sizeof(float));
        solid_smooth_normals(solid, solid->normals, smooth_normals);

        size_t smooth_normals_n = vbo_mesh_append_attributes(mesh, SHADER_ATTRIBUTE_SMOOTH_NORMAL, NORMAL_SIZE, GL_FLOAT, solid->size, smooth_normals);
        log_assert( smooth_normals_n == solid->size );

        free(smooth_normals);
    }

    // - if solid->size is smaller then solid->indices_size, the solid has been optimized or compressed and needs
    // to have indices uploaded to render correctly
    if( solid->size < solid->indices_size ) {
        size_t indices_n = vbo_mesh_append_indices(mesh, solid->indices_size, solid->indices);
        log_assert( indices_n == solid->indices_size );
    }
}

void shitty_triangulate(float* vertices, int32_t n, int32_t m, int32_t* triangles) {
    log_assert( n >= 3 );
    log_assert( n < INT_MAX/3 );
    log_assert( m <= 3 * n - 2);

    if( n == 3 || n == 4 ) {
        triangles[0] = 0;
        triangles[1] = 1;
        triangles[2] = 2;
    }

    // #YOLO
    if( n == 4 ) {
        static bool warn_once = true;
        if( warn_once ) {
            log_warn(__FILE__, __LINE__, "using completely shitty_triangulate function!\n");
            warn_once = false;
        }
        triangles[3] = 2;
        triangles[4] = 3;
        triangles[5] = 0;
    }

    log_assert( n == 3 || n == 4 );
}

void vbo_mesh_create_from_halfedgemesh(const struct HalfEdgeMesh* halfedgemesh, struct Vbo* const vbo, struct Ibo* const ibo, struct VboMesh* mesh) {
    log_assert( halfedgemesh->size >= 0 );

    uint32_t* triangles = malloc(sizeof(uint32_t) * (size_t)halfedgemesh->size);
    uint32_t* optimal = malloc(sizeof(uint32_t) * (size_t)halfedgemesh->size);
    uint32_t* indices = malloc(sizeof(uint32_t) * (size_t)halfedgemesh->size);
    float* vertices = malloc(sizeof(float) * (size_t)halfedgemesh->size*3);
    float* normals = malloc(sizeof(float) * (size_t)halfedgemesh->size*3);
    uint8_t* colors = malloc(sizeof(uint8_t) * (size_t)halfedgemesh->size*4);
    float* texcoords = malloc(sizeof(float) * (size_t)halfedgemesh->size*2);

    log_assert( halfedgemesh->size > 0 );

    // - I use a solid internally because this code originally filled a solid, since it is only used locally
    // and as input for vbo_mesh_create_from_solid below, this is not a problem
    struct Solid solid = {
        .size = (uint32_t)halfedgemesh->size,
        .indices_size = (uint32_t)halfedgemesh->size,

        .triangles = triangles,
        .optimal = optimal,
        .indices = indices,

        .vertices = vertices,
        .normals = normals,
        .colors = colors,
        .texcoords = texcoords
    };

    uint32_t vertices_offset = 0;
    uint32_t normals_offset = 0;
    uint32_t colors_offset = 0;
    uint32_t texcoords_offset = 0;
    uint32_t indices_offset = 0;
    uint32_t optimal_offset = 0;
    uint32_t triangles_offset = 0;

    for( uint32_t i = 0; i < halfedgemesh->faces.occupied; i++ ) {
        struct HalfEdgeFace* face = &halfedgemesh->faces.array[i];

        if( face->edge == -1 ) {
            continue;
        }

        log_assert( face->size >= 0 );

#ifdef CUTE_BUILD_MSVC
        // - the vla approach is actually nice here, but msvc does not support those, so I am just
        // using mallocs inside the loop and free them at the end
        // - these repeated mallocs are not optimal and might be nicer outside the loop, but it is
        // easier to read like this and I don't think it should matter that much
        // - alloca is a bad idea here, you can't free it easily and it will only free when the function
        // goes out of scope, not the loop, so alloca would repeatedly allocate stuff on the stack
        // without releasing it
        float* face_vertices = malloc(sizeof(float) * (size_t)face->size*3);
        float* edge_normals = malloc(sizeof(float) * (size_t)face->size*3);
        uint8_t* edge_colors = malloc(sizeof(uint8_t) * (size_t)face->size*4);
        float* edge_texcoords = malloc(sizeof(float) * (size_t)face->size*2);
        uint32_t* face_triangles = malloc(sizeof(uint32_t) * (size_t)face->size);
#else
        float face_vertices[face->size*3];
        float edge_normals[face->size*3];
        uint8_t edge_colors[face->size*4];
        float edge_texcoords[face->size*2];
        uint32_t face_triangles[face->size];
#endif

        struct HalfEdge* current_edge = &halfedgemesh->edges.array[face->edge];
        for( int32_t face_vertex_i = 0; face_vertex_i < face->size; face_vertex_i++ ) {
            face_vertices[face_vertex_i*3+0] = halfedgemesh->vertices.array[current_edge->vertex].position[0];
            face_vertices[face_vertex_i*3+1] = halfedgemesh->vertices.array[current_edge->vertex].position[1];
            face_vertices[face_vertex_i*3+2] = halfedgemesh->vertices.array[current_edge->vertex].position[2];

            edge_normals[face_vertex_i*3+0] = current_edge->normal[0];
            edge_normals[face_vertex_i*3+1] = current_edge->normal[1];
            edge_normals[face_vertex_i*3+2] = current_edge->normal[2];

            edge_colors[face_vertex_i*4+0] = current_edge->color[0];
            edge_colors[face_vertex_i*4+1] = current_edge->color[1];
            edge_colors[face_vertex_i*4+2] = current_edge->color[2];
            edge_colors[face_vertex_i*4+3] = current_edge->color[3];

            edge_texcoords[face_vertex_i*2+0] = current_edge->texcoord[0];
            edge_texcoords[face_vertex_i*2+1] = current_edge->texcoord[1];

            log_assert( current_edge->vertex >= 0 );
            face_triangles[face_vertex_i] = (uint32_t)current_edge->vertex;

            current_edge = &halfedgemesh->edges.array[current_edge->next];
        }

        int32_t tesselation_size = 3 * (face->size - 2);
#ifdef CUTE_BUILD_MSVC
        int32_t* face_tesselation = malloc(sizeof(int32_t) * (size_t)tesselation_size);
#else
        int32_t face_tesselation[tesselation_size];
#endif
        shitty_triangulate(face_vertices, face->size, tesselation_size, face_tesselation);

        for( int32_t j = 0; j < tesselation_size ; j++ ) {
            int32_t k = face_tesselation[j];

            solid.vertices[vertices_offset+0] = face_vertices[k*3+0];
            solid.vertices[vertices_offset+1] = face_vertices[k*3+1];
            solid.vertices[vertices_offset+2] = face_vertices[k*3+2];
            vertices_offset += 3;

            solid.normals[normals_offset+0] = edge_normals[k*3+0];
            solid.normals[normals_offset+1] = edge_normals[k*3+1];
            solid.normals[normals_offset+2] = edge_normals[k*3+2];
            normals_offset += 3;

            solid.colors[colors_offset+0] = edge_colors[k*4+0];
            solid.colors[colors_offset+1] = edge_colors[k*4+1];
            solid.colors[colors_offset+2] = edge_colors[k*4+2];
            solid.colors[colors_offset+3] = edge_colors[k*4+3];
            colors_offset += 4;

            solid.texcoords[texcoords_offset+0] = edge_texcoords[k*2+0];
            solid.texcoords[texcoords_offset+1] = edge_texcoords[k*2+1];
            texcoords_offset += 2;

            solid.indices[indices_offset] = indices_offset;
            indices_offset += 1;

            solid.optimal[optimal_offset] = face_triangles[k];
            optimal_offset += 1;

            solid.triangles[triangles_offset] = face_triangles[k];
            triangles_offset += 1;
        }

#ifdef CUTE_BUILD_MSVC
        free(face_vertices);
        free(edge_normals);
        free(edge_colors);
        free(edge_texcoords);
        free(face_triangles);
        free(face_tesselation);
#endif
    }

    // - running solid_optimize on the solid before making a vbo_mesh out of it results in cool looking meshes,
    // but not what I expect, the face_triangles don't take into account which vertices have equal normals, so
    // optimizing is currently not possible, or rather its the same as compressing
    //solid_optimize(&solid, &solid);

    // using a solid as input somewhere else is ok
    vbo_mesh_create_from_solid(&solid, vbo, ibo, mesh);

    free(triangles);
    free(optimal);
    free(indices);
    free(vertices);
    free(normals);
    free(colors);
    free(texcoords);
}

void vbo_mesh_render(struct VboMesh* mesh, struct Shader* shader, const struct Camera* camera, const Mat model_matrix) {
    log_assert( mesh != NULL );
    log_assert( shader != NULL );
    log_assert( camera != NULL );
    log_assert( mesh->vbo != NULL);
    log_assert( mesh->vbo->buffer->id > 0 );

    shader_verify_locations(shader);

    shader_use_program(shader);

    Mat projection_matrix = {0};
    Mat view_matrix = {0};
    camera_matrices(camera, CAMERA_PERSPECTIVE, projection_matrix, view_matrix);
    projection_matrix[14] += mesh->z_offset;
    log_assert( shader_set_uniform_matrices(shader, 0, projection_matrix, view_matrix, model_matrix) > -1 );

    GLint loc[MAX_SHADER_ATTRIBUTES] = {0};
    for( int32_t array_id = 0; array_id < MAX_SHADER_ATTRIBUTES; array_id++ ) {
        loc[array_id] = -1;
    }
    bool not_binding_vao = true;

#ifndef CUTE_BUILD_ES2
    if( mesh->vao == 0 ) {
        glGenVertexArrays(1, &mesh->vao);
    } else {
        not_binding_vao = false;
    }
    glBindVertexArray(mesh->vao);
#endif

    for( int32_t array_id = 0; array_id < MAX_SHADER_ATTRIBUTES; array_id++ ) {
        uint32_t c_num = mesh->vbo->components[array_id].size;
        GLenum c_type = mesh->vbo->components[array_id].type;
        uint32_t c_bytes = mesh->vbo->components[array_id].bytes;
        size_t attributes_offset = mesh->attributes.offset * c_num * c_bytes;

        if( c_num == 0 || c_bytes == 0 ) {
            // - when this attribute is not part of the vbo, just continue
            // - there used to be a warning here, when an attribute is not in the vbo, but the shader has a location for it, I got rid of it
            // because I wanted all location warnings to happen in shader_warn_locations instead, it should sufficient that I warn when a shader
            // location has never been set, to notice that something is missing, which I still do
            continue;
        }

        if( shader->attribute[array_id].location > -1 ) {
            if( not_binding_vao ) {
                // - the shader_set_attribute functions call the glVertexAttribPointer function, the attributes_offset is set to indicate the where the meshes
                // attributes start, this offset makes it possible that I can use indices that start from zero
                // - the vertex attrib pointer (and therefore the attributes offset) becomes part of the vao, that means changing it later becomes difficult,
                // if I need that, I could either throw away the vao, thus triggering this code again on the next render, or I could use the glDrawElementsBaseVertex
                // call below instead of the plain glDrawElements, but that would not be possible in opengl es2
                log_assert( c_num < INT_MAX );
                loc[array_id] = shader_set_attribute(shader, array_id, mesh->vbo->buffer[array_id].id, (GLint)c_num, c_type, 0, (void*)(intptr_t)attributes_offset);
            } else {
                // - bit of a hack here: I need to indicate that this attribute would have been set with shader_set_attribute, when using a vao,
                // thats why the if( not_binding_vao ) check was moved from around the for loop, to here
                // - we need to do this so the warning code works correctly, if we use a vao, we set the attributes only once per mesh, and the for every other shader
                // using the same mesh we still may need to indicate that the attributes have been set, even though we don't need to call shader_set_attribute when
                // using a vao
                shader->attribute[array_id].unset = false;
            }
        }
    }

    // - the element array buffer binding should be part of the vao, but it may be not on some drivers (intel):
    // http://stackoverflow.com/questions/8973690/vao-and-element-array-buffer-state
    // - so I could optionally condition on not_binding_vao so that this only done once, for now, this is done
    // always (don't forget same check below to unbind)
    if( mesh->indices.occupied > 0 ) {
        ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo->buffer->id); );
    }

    // - I am passing the loc array so that shader_warn_locations can warn about missing locations for attributes,
    // but only if those attributes are found in the current vbo
    shader_warn_locations(shader, loc);

    if( mesh->indices.occupied > 0 ) {
        // - the 4th argument of glDrawElements is a pointer, but it is only used as a pointer if we are using a client side array, if
        // we use a server side array, a vbo, then it acts like an offset
        // - the indices_offset is the offset in bytes to where the indices start of the mesh we want to render, in the mesh->ibo->buffer->id
        // buffer, we don't have to
        intptr_t indices_offset = mesh->indices.offset * mesh->ibo->index.bytes;
        ogl_debug( glDrawElements(mesh->ibo->primitives.type, mesh->indices.occupied, mesh->ibo->index.type, (void*)indices_offset); );
    } else {
        // - the offset is 0 here because we specify the offset already in the glVertexAttribPointer call above
        // - in this case we'd actually better just render the whole buffer as one batch anyways, and that
        // would be better done in another function
        ogl_debug( glDrawArrays(mesh->ibo->primitives.type, 0, mesh->attributes.occupied[SHADER_ATTRIBUTE_VERTEX]) );
    }

#ifndef CUTE_BUILD_ES2
    if( mesh->vao > 0 ) {
        glBindVertexArray(0);
    }
#endif

    if( not_binding_vao ) {
        for( int32_t array_id = 0; array_id < MAX_SHADER_ATTRIBUTES; array_id++ ) {
            if( loc[array_id] > -1 ) {
                ogl_debug( glDisableVertexAttribArray((GLuint)loc[array_id]) );
            }
        }

        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, 0); );
    }

    if( mesh->indices.occupied > 0 ) {
        ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    }

    shader_use_program(NULL);
}
