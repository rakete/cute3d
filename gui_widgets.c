#include "gui_widgets.h"

void widgets_display_texture(struct Canvas* canvas,
                             int32_t layer,
                             Vec4f cursor,
                             int32_t x, int32_t y,
                             const Mat model_matrix,
                             const Color color,
                             int32_t width,
                             int32_t height,
                             const struct Texture* texture)
{
    /* canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, 14, vertices); */
    /* canvas_append_attributes(canvas, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE, 14, colors); */
    /* canvas_append_attributes(canvas, SHADER_ATTRIBUTE_TEXCOORDS, 2, GL_FLOAT, 14, texcoords); */
    /* canvas_append_attributes(canvas, SHADER_ATTRIBUTE_NEXT_VERTEX, 3, GL_FLOAT, 14, next_vertices); */
    /* canvas_append_attributes(canvas, SHADER_ATTRIBUTE_LINE_THICKNESS, 1, GL_FLOAT, 14, thickness_array); */
    /* canvas_append_indices(canvas, layer_i, CANVAS_NO_TEXTURE, "volumetric_lines_shader", CANVAS_PROJECT_WORLD, GL_TRIANGLES, 12*3, triangles, offset); */
}
