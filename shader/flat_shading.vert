//CUTE
uniform mat4 mvp_matrix;
uniform mat4 normal_matrix;

uniform vec3 light_direction;

shader_in vec3 vertex;
shader_in vec4 diffuse_color;
shader_in vec3 vertex_normal;
shader_in vec2 vertex_texcoord;

shader_out vec2 frag_texcoord;
shader_out vec4 frag_color;
shader_out float intensity;

void main() {
    intensity = 0.5 - dot(vec4(normalize(light_direction), 0.0), normal_matrix * vec4(normalize(vertex_normal),0.0))/2.0;

    gl_Position = mvp_matrix * vec4(vertex,1.0);
    frag_texcoord = vertex_texcoord;
    frag_color = diffuse_color;
}
