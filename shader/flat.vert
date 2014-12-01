#version 130

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;

in vec3 vertex;

in vec4 color;
smooth out vec4 frag_color;

in vec3 normal;
uniform vec3 light_direction;

smooth out float intensity;

void main() {
    intensity = (1 + dot(light_direction, normal)) / 2;
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex,1.0);
    frag_color = color;
}
