//CUTE
uniform sampler2D diffuse_texture;

uniform vec4 specular_light;
uniform vec4 ambient_light;

uniform int enable_texture;

shader_in vec2 frag_texcoord;
shader_in vec4 frag_color;
shader_in float intensity;

void main() {
    // - instead of using an if, I could do this:
    //gl_FragColor = mix(frag_color, texture2D(diffuse_texture, vec2(frag_texcoord.x,frag_texcoord.y)), float(enable_texture > 0));
    vec4 color_ambient = mix(ambient_light, frag_color, intensity);
    vec4 color_specular = mix(specular_light, frag_color, 1.2 - (intensity)); //1.2 - (intensity)
    vec4 color = mix(color_ambient, color_specular, intensity);
    color.xyz -= 0.05;
    gl_FragColor = color;
}
