uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;

in vec3 vertex;

in vec4 color;
smooth out vec4 frag_color;

mat4 identity = mat4(vec4(1,0,0,0),
                     vec4(0,1,0,0),
                     vec4(0,0,1,0),
                     vec4(0,0,0,1));

mat4 translation = mat4(vec4(1,0,0,0),
                        vec4(0,1,0,0),
                        vec4(0,0,1,0),
                        vec4(0,1,0,1));

mat4 perspective = mat4(vec4(1.0/tan(45.0), 0               , 0, 0),
                        vec4(0            , 1.3333/tan(45.0), 0, 0),
                        vec4(0            , 0               , 1, 0),
                        vec4(0            , 0               , 0, 1));

void main() {
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex,1.0);
    frag_color = color;
}
