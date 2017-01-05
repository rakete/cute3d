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

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform vec3 light_position;
uniform float light_attenuation;
uniform vec3 eye_position;

uniform float material_shininess;
uniform vec4 material_coefficients;

shader_in vec3 vertex;
shader_in vec3 vertex_normal;
shader_in vec2 vertex_texcoord;

shader_in vec4 vertex_color;

shader_out vec2 frag_texcoord;
shader_out vec4 frag_color;
shader_out float intensity;

void main() {
    vec3 world_position = mat3(model_matrix) * vertex;
    vec3 world_normal = normalize(mat3(model_matrix) * vertex_normal);

    //don't forget to normalize
    vec3 light_direction = normalize(light_position - world_position);
    vec3 view_direction = normalize(eye_position - world_position);

    //Lambert term
    float lambert_term = max(0.0, dot(light_direction, world_normal));

    //consider diffuse light color white(1,1,1)
    //all color channels have the same float value
    float material_kd = material_coefficients[0];
    float diffuse = material_kd * lambert_term;

    float material_ks = material_coefficients[1];
    float specular = 0.0;
    float ambient = 0.0;
    if( lambert_term > 0.0 ) {
        //can use built-in max or saturate function
        vec3 reflection = -normalize(reflect(light_direction, world_normal));
        specular = material_ks * pow( max(0.0, dot(reflection, view_direction)), material_shininess);
    }

    //pass light to fragment shader
    //intensity = diffuse + specular;

    float material_ke = material_coefficients[3];
    float material_ka = material_coefficients[2];
    //float ambient = 0.5 - dot(vec4(normalize(light_direction), 0.0), normal_matrix * vec4(normalize(vertex_normal),0.0))/2.0;
    //How about with ambinetal and emissive?
    //Final light(with white(1,1,1)) would be:
    intensity = material_ke + material_ka + diffuse + specular;

    //final vertex position
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex, 1);
    frag_texcoord = vertex_texcoord;
    frag_color = vertex_color;
}
