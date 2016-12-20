//CUTE
uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;

shader_in vec3 vertex;
shader_in vec4 vertex_color;
shader_in vec2 vertex_texcoord;

shader_out vec2 frag_texcoord;
shader_out vec4 frag_color;
void main() {
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex,1.0);
    frag_texcoord = vertex_texcoord;
    frag_color = vertex_color;
}
