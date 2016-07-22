//CUTE
uniform mat4 mvp_matrix;

shader_in vec3 vertex;
shader_in vec4 color;

shader_out vec4 frag_color;
void main() {
    gl_Position = mvp_matrix * vec4(vertex,1.0);
    frag_color = color;
}
