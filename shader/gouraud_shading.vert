//CUTE
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform vec4 specular_light;
uniform vec4 ambient_light;

uniform vec3 light_position;
uniform float light_attenuation;
uniform vec3 eye_position;

uniform float material_shininess;
uniform vec3 material_coefficients;

shader_in vec3 vertex;
shader_in vec3 vertex_normal;
shader_in vec2 vertex_texcoord;

shader_in vec4 diffuse_color;

shader_out vec2 frag_texcoord;
shader_out vec4 frag_color;
shader_out float intensity;

void main() {
    vec3 world_position = mat3(model_matrix) * vertex;
    vec3 world_normal = normalize(mat3(model_matrix) * vertex_normal);

    //don't forget to normalize
    vec3 light_direction = normalize(light_position - world_position);
    vec3 view_direction = normalize(eye_position - world_position);

    //Lambert term
    float lambert_term = max(0.0, dot(light_direction, world_normal));

    //consider diffuse light color white(1,1,1)
    //all color channels have the same float value
    float material_kd = material_coefficients[0];
    float diffuse = material_kd * lambert_term;

    float material_ks = material_coefficients[1];
    float specular = 0.0;
    if( lambert_term > 0.0 ) {
        //can use built-in max or saturate function
        vec3 reflection = -normalize(reflect(light_direction, world_normal));
        specular = material_ks * pow( max(0.0, dot(reflection, view_direction)), material_shininess);
    }

    //pass light to fragment shader
    intensity = diffuse + specular;

    //float material_ka = material_coefficients[2];
    //How about with ambinetal and emissive?
    //Final light(with white(1,1,1)) would be:
    //light = ke + material_ka + diffuse + specular;

    //final vertex position
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex, 1);
    frag_texcoord = vertex_texcoord;
    frag_color = diffuse_color;
}