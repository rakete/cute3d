//CUTE
uniform sampler2D diffuse_texture;

shader_in vec4 frag_color;
shader_in float intensity;

uniform vec4 ambient_color;
uniform int enable_texture;

void main() {
    // - instead of using an if, I could do this:
    //gl_FragColor = mix(frag_color, texture2D(diffuse_texture, vec2(frag_texcoord.x,frag_texcoord.y)), float(enable_texture > 0));

    gl_FragColor = ambient_color*(1.0 - intensity) + frag_color*intensity;
    //gl_FragColor = ambient_color*intensity;
}
