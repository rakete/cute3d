//CUTE
uniform sampler2D diffuse_texture;

uniform vec4 ambient_light;

uniform int enable_texture;

shader_in vec2 frag_texcoord;
shader_in vec4 frag_color;
shader_in float intensity;

void main() {
    // - instead of using an if, I could do this:
    //gl_FragColor = mix(frag_color, texture2D(diffuse_texture, vec2(frag_texcoord.x,frag_texcoord.y)), float(enable_texture > 0));

    gl_FragColor = mix( mix( vec4(ambient_light[3]*frag_color[0], ambient_light[3]*frag_color[1], ambient_light[3]*frag_color[2], frag_color[3]),
                             vec4(ambient_light[0], ambient_light[1], ambient_light[2], 1),
                             ambient_light[3] ),
                        frag_color,
                        intensity );
}
