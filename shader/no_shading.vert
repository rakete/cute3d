//CUTE
uniform mat4 mvp_matrix;

shader_in vec3 vertex;
shader_in vec4 color;
shader_in vec2 texcoord;

shader_out vec4 frag_color;
shader_out vec2 frag_texcoord;
void main() {
    gl_Position = mvp_matrix * vec4(vertex,1.0);
    frag_color = color;
    frag_texcoord = texcoord;
}
