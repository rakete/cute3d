//CUTE_VERTEX

// Cute3D, a simple opengl based framework for writing interactive realtime applications

// Copyright (C) 2013-2017 Andreas Raster

// This file is part of Cute3D.

// Cute3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Cute3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Cute3D.  If not, see <http://www.gnu.org/licenses/>.

uniform mat4 mvp_matrix;

shader_in vec3 vertex;
shader_in vec4 vertex_color;
shader_in vec2 vertex_texcoord;
shader_in vec3 next_vertex;
shader_in float line_thickness;

shader_out vec4 frag_color;
shader_out vec2 frag_texcoord;

uniform float aspect_ratio;

void main()
{
    float safe_aspect = 1280.0/720.0;
    if( aspect_ratio > 0.0 ) {
        safe_aspect = aspect_ratio;
    }

    frag_texcoord = vertex_texcoord;
    frag_color = vertex_color;

    //compute vertices position in clip space
    vec4 current_projected = mvp_matrix * vec4(vertex, 1.0);
    vec4 next_projected = mvp_matrix * vec4(next_vertex, 1.0);

    float dir_correction = sign(line_thickness);

    //  line direction in screen space (perspective division required)
    vec2 line_dir = dir_correction * abs(line_thickness) * normalize(current_projected.xy/current_projected.ww - next_projected.xy/next_projected.ww);

    // small trick to avoid inversed line condition when points are not on the same side of Z plane
    bool same_side_z = sign(next_projected.w) == sign(current_projected.w);
    if( ! same_side_z ) {
        line_dir = -line_dir;
    }

    // offset position in screen space along line direction and orthogonal direction
    vec2 offset_x = line_dir.xy * vertex_texcoord.xx * vec2(1.0, safe_aspect);
    vec2 offset_y = line_dir.yx * vec2(1.0, -1.0) * vertex_texcoord.yy * vec2(1.0, safe_aspect);
    current_projected.xy += offset_x;
    current_projected.xy += offset_y;

    gl_Position = current_projected;
}
