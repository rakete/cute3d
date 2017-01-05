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

shader_in vec3 next_vertex;
shader_in vec3 prev_vertex;
shader_in float line_thickness;

shader_out vec4 frag_color;

uniform float aspect_ratio;
uniform int disable_miter;

void main() {
    float safe_aspect = 1280.0/720.0;
    if( aspect_ratio > 0.0 ) {
        safe_aspect = aspect_ratio;
    }

    vec2 aspect_vec = vec2(safe_aspect, 1.0);
    vec4 prev_projected = mvp_matrix * vec4(prev_vertex, 1.0);
    vec4 current_projected = mvp_matrix * vec4(vertex, 1.0);
    vec4 next_projected = mvp_matrix * vec4(next_vertex, 1.0);

    // - get 2D screen space with W divide and aspect correction
    vec2 current_screen = (current_projected.xy / current_projected.w) * aspect_vec;
    vec2 prev_screen = (prev_projected.xy / prev_projected.w) * aspect_vec;
    vec2 next_screen = (next_projected.xy / next_projected.w) * aspect_vec;

    float temp_thickness = line_thickness;

    vec2 dir = vec2(0,0);
    if( current_screen == prev_screen ) {
        // - starting point uses (next - current)
        dir = normalize(next_screen - current_screen);
    } else if (current_screen == next_screen) {
        // - ending point uses (current - previous)
        dir = normalize(current_screen - prev_screen);
    } else {
        // - somewhere in middle, needs a join
        // - get directions from (C - B) and (B - A)
        vec2 edgeA = current_screen - prev_screen;
        vec2 dirA = normalize(edgeA);
        if( disable_miter == 0 ) {
            vec2 edgeB = next_screen - current_screen;
            vec2 dirB = normalize(edgeB);

            // - now compute the miter join normal and length
            vec2 tangent = normalize(dirA + dirB);
            vec2 perp = vec2(-dirA.y, dirA.x);
            vec2 miter_vec = vec2(-tangent.y, tangent.x);

            float miter_adjust = dot(miter_vec, perp);
            if( abs(miter_adjust) < line_thickness/5.0 ) {
                dir = vec2(line_thickness * 2.0, line_thickness * 2.0);
            } else {
                dir = tangent;
                temp_thickness = line_thickness / abs(miter_adjust);
            }

        } else {
            dir = dirA;
        }
    }

    vec2 normal = vec2(-dir.y, dir.x);
    normal *= temp_thickness/2.0;
    normal.x /= safe_aspect;

    vec4 offset = vec4(normal, 0.0, 0.0)*(current_projected.z/10.0);
    gl_Position = current_projected + offset;
    gl_PointSize = 1.0;

    frag_color = vertex_color;
}
