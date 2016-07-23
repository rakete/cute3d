#include "texture_pattern.h"

#define PATTERN_COLOR_SIZE 4

/*  0:0  1:0  2:1  3:1
    4:0  5:0  6:1  7:1
    8:1  9:1 10:0 11:0
   12:1 13:1 14:0 15:0 */

void pattern_checkerboard(uint32_t tile_width, uint32_t tile_height, uint32_t board_width, uint32_t board_height, Color color1, Color color2, uint8_t* pattern) {
    size_t board_size = board_width * board_height * tile_width * tile_height * PATTERN_COLOR_SIZE;
    log_assert( pattern[board_size-1] == 0 );

    size_t line_size = tile_width * board_width * PATTERN_COLOR_SIZE;

    for( size_t board_y = 0; board_y < board_height; board_y++ ) {
        size_t offset_board_y = board_y*tile_height*line_size;
        for( size_t board_x = 0; board_x < board_width; board_x++ ) {
            size_t offset_board_xy = offset_board_y + board_x*tile_width*PATTERN_COLOR_SIZE;
            for( size_t tile_y = 0; tile_y < tile_height; tile_y++ ) {
                size_t offset_tile_y_board_xy = offset_board_xy + tile_y*line_size;
                for( size_t tile_x = 0; tile_x < tile_height; tile_x++ ) {
                    size_t pixel_i = offset_tile_y_board_xy + tile_x*PATTERN_COLOR_SIZE;

                    uint32_t color_select = 0;
                    if( board_y % 2 > 0 ) {
                        color_select = 1;
                    }

                    if( board_x % 2 == color_select ) {
                        //printf("%zu %zu 0\n", pixel_i/4, pixel_i);
                        color_copy(color1, &pattern[pixel_i]);
                    } else {
                        //printf("%zu %zu 1\n", pixel_i/4, pixel_i);
                        color_copy(color2, &pattern[pixel_i]);
                    }
                }
            }
        }
    }
}
