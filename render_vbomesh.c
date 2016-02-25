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

#include "render_vbomesh.h"

void vbomesh_create_from_solid(const struct Solid* solid, struct Vbo* vbo, struct VboMesh* mesh) {
    log_assert( solid != NULL );
    log_assert( mesh != NULL );

    vbomesh_create(vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, mesh);

    size_t vertices_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_VERTICES, solid->vertices, 3, GL_FLOAT, solid->size);
    log_assert( vertices_n == solid->size );

    if( vbo->buffer[SHADER_ATTRIBUTE_NORMALS].id ) {
        size_t normals_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_NORMALS, solid->normals, 3, GL_FLOAT, solid->size);
        log_assert( normals_n == solid->size );
    }

    if( vbo->buffer[SHADER_ATTRIBUTE_COLORS].id ) {
        size_t colors_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_COLORS, solid->colors, 4, GL_UNSIGNED_BYTE, solid->size);
        log_assert( colors_n == solid->size );
    }

    if( vbo->buffer[SHADER_ATTRIBUTE_TEXCOORDS].id ) {
        size_t texcoords_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_TEXCOORDS, solid->texcoords, 2, GL_FLOAT, solid->size);
        log_assert( texcoords_n == solid->size );
    }

    if( solid->size < solid->indices_size ) {
        size_t indices_n = vbomesh_append_indices(mesh, solid->indices, solid->indices_size);
        log_assert( indices_n == solid->indices_size );
    }
}
}

void vbomesh_render(const struct VboMesh* mesh, const struct Shader* shader, const struct Camera* camera, const Mat model_matrix) {
    log_assert( mesh != NULL );
    log_assert( shader != NULL );
    log_assert( camera != NULL );

    ogl_debug( glUseProgram(shader->program); );

    Mat projection_matrix = {0};
    Mat view_matrix = {0};
    camera_matrices(camera, CAMERA_PERSPECTIVE, projection_matrix, view_matrix);
    log_assert( shader_set_uniform_matrices(shader, projection_matrix, view_matrix, model_matrix) > -1 );

    GLint loc[NUM_SHADER_ATTRIBUTES] = {0};
    for( int32_t array_id = 0; array_id < NUM_SHADER_ATTRIBUTES; array_id++ ) {
        uint32_t c_num = mesh->vbo->components[array_id].size;
        GLenum c_type = mesh->vbo->components[array_id].type;
        uint32_t c_bytes = mesh->vbo->components[array_id].bytes;
        size_t offset = mesh->offset * c_num * c_bytes;

        loc[array_id] = -1;
        if( c_num == 0 || c_bytes == 0 ) {
            continue;
        }

        if( shader->attribute[array_id].location > -1 ) {
            loc[array_id] = shader->attribute[array_id].location;
        } else if( strlen(shader->attribute[array_id].name) > 0 ) {
            ogl_debug( loc[array_id] = glGetAttribLocation(shader->program, shader->attribute[array_id].name) );
            log_warn(stderr, __FILE__, __LINE__, "attribute %d location \"%s\" of shader \"%s\" not cached\n", array_id, shader->attribute[array_id].name, shader->name);
        } else {
            continue;
        }

        log_assert( c_num <= 4, "%d <= 4: glVertexAttribPointer will not work with attribute component size > 4\n", c_num);
        if( mesh->vbo->buffer[array_id].id && loc[array_id] > -1 ) {
            ogl_debug( glEnableVertexAttribArray((GLuint)loc[array_id]);
                       glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[array_id].id);
                       // - the offset here is what makes things work out with the indices starting at 0
                       // - I came here from trying to figure out why I had *triangles and *indices in solids,
                       // the reason is that most of my solids simply don't need indices and render fine with
                       // just a glDrawArrays call
                       glVertexAttribPointer((GLuint)loc[array_id], (GLint)c_num, c_type, GL_TRUE, 0, (void*)(intptr_t)offset) );
        }
    }

    if( mesh->indices->id ) {
        ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices->id); );
        ogl_debug( glDrawElementsBaseVertex(mesh->primitives.type, mesh->indices->occupied, mesh->index.type, 0, mesh->indices->base); );
        ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    } else {
        // - the offset is 0 here because we specify the offset already in the glVertexAttribPointer call above
        // - in this case we'd actually better just render the whole buffer as one batch anyways, and that
        // would be better done in another function
        ogl_debug( glDrawArrays(mesh->primitives.type, 0, mesh->occupied[SHADER_ATTRIBUTE_VERTICES]) );
    }

    for( int32_t array_id = 0; array_id < NUM_SHADER_ATTRIBUTES; array_id++ ) {
        if( loc[array_id] > -1 ) {
            ogl_debug( glDisableVertexAttribArray((GLuint)loc[array_id]) );
        }
    }

    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, 0);
               glUseProgram(0); );
}
