#include "gui_widgets.h"

#define WIDGETS_VERTEX_SIZE 3
#define WIDGETS_COLOR_SIZE 4
#define WIDGETS_TEXCOORD_SIZE 2

void widgets_display_texture(struct Canvas* canvas,
                             int32_t layer_i,
                             int32_t x, int32_t y,
                             int32_t width,
                             int32_t height,
                             const char* name,
                             struct Texture texture)
{
    if( x < 0 ) x = canvas->width + x;
    if( y < 0 ) y = canvas->height + y;

    float vertices[4*WIDGETS_VERTEX_SIZE] = {
              x,        -1*y, 0.0f,
        x+width,        -1*y, 0.0f,
        x+width, -1*y-height, 0.0f,
              x, -1*y-height, 0.0f
    };

    float texcoords[4*WIDGETS_TEXCOORD_SIZE] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    uint32_t indices[6] = {0};
    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].occupied;
    indices[0] = offset + 3;
    indices[1] = offset + 1;
    indices[2] = offset + 0;
    indices[3] = offset + 3;
    indices[4] = offset + 2;
    indices[5] = offset + 1;

    static int32_t texture_i = MAX_CANVAS_TEXTURES;
    if( texture_i == MAX_CANVAS_TEXTURES ) {
        texture_i = canvas_add_texture(canvas, SHADER_SAMPLER_DIFFUSE_TEXTURE, name, &texture);
        log_assert( texture_i < MAX_CANVAS_TEXTURES );
    }

    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, 4, vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, 4, texcoords);
    canvas_append_indices(canvas, layer_i, texture_i, "default_shader", CANVAS_PROJECT_SCREEN, GL_TRIANGLES, 2*3, indices, 0);
}
