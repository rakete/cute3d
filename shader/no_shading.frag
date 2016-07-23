//CUTE
uniform sampler2D diffuse_texture;

shader_in vec4 frag_color;
shader_in vec2 frag_texcoord;

uniform int enable_texture;

void main() {
    if( enable_texture > 0 ) {
        //gl_FragColor = vec4(1.0, 0, 0, 1.0);
        vec4 diffuse_value = texture2D(diffuse_texture, vec2(frag_texcoord.x,frag_texcoord.y));
        gl_FragColor = vec4(diffuse_value[0]*frag_color[0],
                            diffuse_value[1]*frag_color[1],
                            diffuse_value[2]*frag_color[2],
                            diffuse_value[3]*frag_color[3]);
    } else {
        gl_FragColor = frag_color;
    }
}
