//CUTE
uniform sampler2D diffuse_texture;

shader_in vec4 frag_color;
shader_in float intensity;

uniform vec4 ambient_color;
uniform int use_texture;

void main() {
    gl_FragColor = ambient_color*(1.0 - intensity) + frag_color*intensity;
    //gl_FragColor = ambient_color*intensity;
}
