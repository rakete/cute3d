#ifndef GUI_WIDGETS_H
#define GUI_WIDGETS_H

#include "gui_canvas.h"

void widgets_display_texture(struct Canvas* canvas,
                             int32_t layer,
                             Vec4f cursor,
                             int32_t x, int32_t y,
                             const Mat model_matrix,
                             const Color color,
                             int32_t width,
                             int32_t height,
                             const struct Texture* texture);


#endif
