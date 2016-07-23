#ifndef GUI_WIDGETS_H
#define GUI_WIDGETS_H

#include "gui_canvas.h"

void widgets_display_texture(struct Canvas* canvas,
                             int32_t layer_i,
                             int32_t x, int32_t y,
                             int32_t width,
                             int32_t height,
                             const char* name,
                             struct Texture texture);


#endif
