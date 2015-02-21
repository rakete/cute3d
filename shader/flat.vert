#version 130

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat4 normal_matrix;
uniform mat4 view_matrix;

in vec3 vertex;

in vec4 color;
smooth out vec4 frag_color;

in vec3 normal;
uniform vec3 light_direction;

smooth out float intensity;

void main() {
    mat4 mvp = projection_matrix * view_matrix * model_matrix;

    intensity = (1.0 + -1.0 * dot(vec4(normalize(light_direction), 0.0),
                                  normalize(normal_matrix * vec4(normal,0.0)))) / 2.0;

    gl_Position = mvp * vec4(vertex,1.0);

    frag_color = color;
}
