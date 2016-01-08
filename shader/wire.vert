//CUTE
#version 130
#extension GL_ARB_uniform_buffer_object:require

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;

in vec3 vertex;

void main() {
    gl_Position = projection_matrix * model_matrix * view_matrix * vec4(vertex,1.0);
}
