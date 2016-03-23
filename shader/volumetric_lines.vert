//CUTE
uniform mat4 mvp_matrix;

shader_in vec3 vertex;
shader_in vec4 color;
shader_in vec2 texcoord;
shader_in vec3 next_vertex;
shader_in float line_thickness;

shader_out vec4 frag_color;
shader_out vec2 frag_texcoord;

uniform float aspect_ratio;
uniform float line_z_scaling;

void main()
{
    float safe_aspect = 1280.0/720.0;
    if( aspect_ratio > 0.0 ) {
        safe_aspect = aspect_ratio;
    }

    frag_texcoord = texcoord;
    frag_color = color;

    //compute vertices position in clip space
    vec4 current_projected = mvp_matrix * vec4(vertex, 1.0);
    vec4 next_projected = mvp_matrix * vec4(next_vertex, 1.0);

    //  line direction in screen space (perspective division required)
    vec2 line_dir = line_thickness * normalize(current_projected.xy/current_projected.ww - next_projected.xy/next_projected.ww);

    // small trick to avoid inversed line condition when points are not on the same side of Z plane
    if( sign(next_projected.w) != sign(current_projected.w) ) {
        line_dir = -line_dir;
    }

    // offset position in screen space along line direction and orthogonal direction
    vec2 offset_x = line_dir.xy * texcoord.xx * vec2(1.0, safe_aspect);
    vec2 offset_y = line_dir.yx * vec2(1.0, -1.0) * texcoord.yy * vec2(1.0, safe_aspect);
    if( line_z_scaling > 0.0 ) {
        float scaling = current_projected.z/(line_z_scaling*10.0);
        if( scaling < 0.2 ) {
            scaling = 0.2;
        }
        if( scaling > 5.0 ) {
            scaling = 5.0;
        }
        offset_x *= scaling;
        offset_y *= scaling;
    }
    current_projected.xy += offset_x;
    current_projected.xy += offset_y;

    gl_Position = current_projected;
}
