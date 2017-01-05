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
uniform mat4 normal_matrix;

uniform vec3 light_direction;

shader_in vec3 vertex;
shader_in vec2 vertex_texcoord;
shader_in vec3 vertex_normal;
shader_in vec4 vertex_color;

shader_out vec2 frag_texcoord;
shader_out vec4 frag_color;
shader_out float intensity;

void main() {
    intensity = 0.5 - dot(vec4(normalize(light_direction), 0.0), normal_matrix * vec4(normalize(vertex_normal),0.0))/2.0;

    gl_Position = mvp_matrix * vec4(vertex,1.0);
    frag_texcoord = vertex_texcoord;
    frag_color = vertex_color;
}
