//CUTE
shader_in vec4 frag_color;
shader_in vec2 frag_texcoord;

void main() {
    const float transparency_threshold = 1.0;
    if( length(frag_texcoord) > transparency_threshold ) {
        // - eh, this is probably much more complex then neccessary, that the line transparency goes from
        // fully opaque at the threshold to fully transparent at 1.0, so if threshold is 0.6, then the
        // transparent section fades gradually from 0.6 to 1.0
        // - the length(frag_texcoord) in there is a measurement how far along the lines thickness/2.0 we are,
        // for this current fragment, should be max 1.0 everywhere when we use half circles at the endpoints
        float a = (1.0/(1.0 - transparency_threshold)) * ((1.0 - transparency_threshold) - (length(frag_texcoord) - transparency_threshold));
        gl_FragColor = vec4(frag_color.xyz, a);
    } else {
        gl_FragColor = frag_color;
    }
}
