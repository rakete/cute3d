//CUTE
#version 130

smooth in vec4 frag_color;
smooth in float intensity;

uniform vec4 ambiance;

void main() {
    gl_FragColor = ambiance*(1 - intensity) + frag_color*intensity;
}
