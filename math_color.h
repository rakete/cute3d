/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef MATH_COLOR_H
#define MATH_COLOR_H

#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"

#include "math_types.h"

#define COLOR_SIZE 4

#define COLOR_TYPE uint8_t

typedef uint8_t Color[COLOR_SIZE];
typedef uint8_t ColorP;

struct ColorPaletteEntry {
    uint32_t index;
    const char name[256];
    Color rgb;
};

#define COLOR_EGA_RED (struct ColorPaletteEntry){ .index = 0, .name = "red", .rgb = {255, 0, 0, 255} }
#define COLOR_EGA_BLUE (struct ColorPaletteEntry){ .index = 1, .name = "blue", .rgb = {0, 0, 255, 255} }
#define COLOR_EGA_GREEN (struct ColorPaletteEntry){ .index = 2, .name = "green", .rgb = {0, 255, 0, 255} }
#define COLOR_EGA_YELLOW (struct ColorPaletteEntry){ .index = 3, .name = "yellow", .rgb = {255, 255, 0, 255} }
#define COLOR_EGA_CYAN (struct ColorPaletteEntry){ .index = 4, .name = "cyan", .rgb = {0, 255, 255, 255} }
#define COLOR_EGA_MAGENTA (struct ColorPaletteEntry){ .index = 5, .name = "magenta", .rgb = {255, 0, 255, 255} }
#define NUM_EGA_COLORS 6

extern struct ColorPaletteEntry global_color_palette_EGA[NUM_EGA_COLORS];

#define COLOR_NES_BOULDER (struct ColorPaletteEntry){ .index = 0, .name = "boulder", .rgb = {124, 124, 124, 255} }
#define COLOR_NES_BLUE (struct ColorPaletteEntry){ .index = 1, .name = "blue", .rgb = {0, 0, 252, 255} }
#define COLOR_NES_DUKE_BLUE (struct ColorPaletteEntry){ .index = 2, .name = "duke_blue", .rgb = {0, 0, 188, 255} }
#define COLOR_NES_PURPLE_HEART (struct ColorPaletteEntry){ .index = 3, .name = "purple_heart", .rgb = {68, 40, 188, 255} }
#define COLOR_NES_FLIRT (struct ColorPaletteEntry){ .index = 4, .name = "flirt", .rgb = {148, 0, 132, 255} }
#define COLOR_NES_CARMINE (struct ColorPaletteEntry){ .index = 5, .name = "carmine", .rgb = {168, 0, 32, 0} }
#define COLOR_NES_MARIO (struct ColorPaletteEntry){ .index = 6, .name = "mario", .rgb = {168, 16, 0, 255} }
#define COLOR_NES_RED_BERRY (struct ColorPaletteEntry){ .index = 7, .name = "red_berry", .rgb = {136, 20, 0, 255} }
#define COLOR_NES_SADDLE_BROWN (struct ColorPaletteEntry){ .index = 8, .name = "saddle_brown", .rgb = {80, 48, 0, 255} }
#define COLOR_NES_JAPANESE_LAUREL (struct ColorPaletteEntry){ .index = 9, .name = "japanese_laurel", .rgb = {0, 120, 0, 255} }
#define COLOR_NES_CAMARONE (struct ColorPaletteEntry){ .index = 10, .name = "camarone", .rgb = {0, 104, 0, 255} }
#define COLOR_NES_CRUSOE (struct ColorPaletteEntry){ .index = 11, .name = "crusoe", .rgb = {0, 88, 0, 255} }
#define COLOR_NES_ASTRONAUT_BLUE (struct ColorPaletteEntry){ .index = 12, .name = "astronaut_blue", .rgb = {0, 64, 88, 255} }
#define COLOR_NES_BLACK (struct ColorPaletteEntry){ .index = 13, .name = "black", .rgb = {0, 0, 0, 255} }
#define COLOR_NES_SILVER (struct ColorPaletteEntry){ .index = 14, .name = "silver", .rgb = {188, 188, 188, 255} }
#define COLOR_NES_AZURE_RADIANCE (struct ColorPaletteEntry){ .index = 15, .name = "azure_radiance", .rgb = {0, 120, 248, 255} }
#define COLOR_NES_BLUE_RIBBON (struct ColorPaletteEntry){ .index = 16, .name = "blue_ribbon", .rgb = {0, 88, 248, 255} }
#define COLOR_NES_ELECTRIC_VIOLET (struct ColorPaletteEntry){ .index = 17, .name = "electric_violet", .rgb = {104, 68, 252, 255} }
#define COLOR_NES_PURPLE_PIZZAZZ (struct ColorPaletteEntry){ .index = 18, .name = "purple_pizzazz", .rgb = {216, 0, 204, 255} }
#define COLOR_NES_RAZZMATAZZ (struct ColorPaletteEntry){ .index = 19, .name = "razzmatazz", .rgb = {228, 0, 88, 255} }
#define COLOR_NES_SCARLET (struct ColorPaletteEntry){ .index = 20, .name = "scarlet", .rgb = {248, 56, 0, 255} }
#define COLOR_NES_CHRISTINE (struct ColorPaletteEntry){ .index = 21, .name = "christine", .rgb = {228, 92, 16, 255} }
#define COLOR_NES_PIRATE_GOLD (struct ColorPaletteEntry){ .index = 22, .name = "pirate_gold", .rgb = {172, 124, 0, 255} }
#define COLOR_NES_GREEN (struct ColorPaletteEntry){ .index = 23, .name = "green", .rgb = {0, 184, 0, 255} }
#define COLOR_NES_ISLAMIC_GREEN (struct ColorPaletteEntry){ .index = 24, .name = "islamic_green", .rgb = {0, 168, 0, 255} }
#define COLOR_NES_GREEN_HAZE (struct ColorPaletteEntry){ .index = 25, .name = "green_haze", .rgb = {0, 168, 68, 255} }
#define COLOR_NES_FIJI_ISLAND (struct ColorPaletteEntry){ .index = 26, .name = "fiji_island", .rgb = {0, 136, 136, 255} }
#define COLOR_NES_SMOKE (struct ColorPaletteEntry){ .index = 27, .name = "smoke", .rgb = {248, 248, 248, 255} }
#define COLOR_NES_DODGER_BLUE (struct ColorPaletteEntry){ .index = 28, .name = "dodger_blue", .rgb = {60, 188, 252, 255} }
#define COLOR_NES_CORNFLOWER_BLUE (struct ColorPaletteEntry){ .index = 29, .name = "cornflower_blue", .rgb = {104, 136, 252, 255} }
#define COLOR_NES_LAVENDER_PURPLE (struct ColorPaletteEntry){ .index = 30, .name = "lavender_purple", .rgb = {152, 120, 248, 255} }
#define COLOR_NES_BLUSH_PINK (struct ColorPaletteEntry){ .index = 31, .name = "blush_pink", .rgb = {248, 120, 248, 255} }
#define COLOR_NES_BRILLIANT_ROSE (struct ColorPaletteEntry){ .index = 32, .name = "brilliant_rose", .rgb = {248, 88, 152, 255} }
#define COLOR_NES_BITTERSWEET (struct ColorPaletteEntry){ .index = 33, .name = "bittersweet", .rgb = {248, 120, 88, 255} }
#define COLOR_NES_NEON_CARROT (struct ColorPaletteEntry){ .index = 34, .name = "neon_carrot", .rgb = {252, 160, 68, 255} }
#define COLOR_NES_SELECTIVE_YELLOW (struct ColorPaletteEntry){ .index = 35, .name = "selective_yellow", .rgb = {248, 184, 0, 255} }
#define COLOR_NES_LIME (struct ColorPaletteEntry){ .index = 36, .name = "lime", .rgb = {184, 248, 24, 255} }
#define COLOR_NES_MALACHITE (struct ColorPaletteEntry){ .index = 37, .name = "malachite", .rgb = {88, 216, 84, 255} }
#define COLOR_NES_SPRING_GREEN (struct ColorPaletteEntry){ .index = 38, .name = "spring_green", .rgb = {88, 248, 152, 255} }
#define COLOR_NES_BRIGHT_TURQUOISE (struct ColorPaletteEntry){ .index = 39, .name = "bright_turquoise", .rgb = {0, 232, 216, 255} }
#define COLOR_NES_BATTLESHIP (struct ColorPaletteEntry){ .index = 40, .name = "battleship", .rgb = {120, 120, 120, 255} }
#define COLOR_NES_WHITE_ALABASTER (struct ColorPaletteEntry){ .index = 41, .name = "white_alabaster", .rgb = {252, 252, 252, 255} }
#define COLOR_NES_FRESH_AIR (struct ColorPaletteEntry){ .index = 42, .name = "fresh_air", .rgb = {164, 228, 252, 255} }
#define COLOR_NES_PERFUME (struct ColorPaletteEntry){ .index = 43, .name = "perfume", .rgb = {184, 184, 248, 255} }
#define COLOR_NES_MAUVE (struct ColorPaletteEntry){ .index = 44, .name = "mauve", .rgb = {216, 184, 248, 255} }
#define COLOR_NES_LAVENDER_PINK (struct ColorPaletteEntry){ .index = 45, .name = "lavender_pink", .rgb = {248, 184, 248, 255} }
#define COLOR_NES_FLAMINGO_PINK (struct ColorPaletteEntry){ .index = 46, .name = "flamingo_pink", .rgb = {248, 164, 192, 255} }
#define COLOR_NES_DESERT_SAND (struct ColorPaletteEntry){ .index = 47, .name = "desert_sand", .rgb = {240, 208, 176, 255} }
#define COLOR_NES_PEACH_YELLOW (struct ColorPaletteEntry){ .index = 48, .name = "peach_yellow", .rgb = {252, 224, 168, 255} }
#define COLOR_NES_MELLOW_YELLOW (struct ColorPaletteEntry){ .index = 49, .name = "mellow_yellow", .rgb = {248, 216, 120, 255} }
#define COLOR_NES_MIDORI (struct ColorPaletteEntry){ .index = 50, .name = "midori", .rgb = {216, 248, 120, 255} }
#define COLOR_NES_PALE_GREEN (struct ColorPaletteEntry){ .index = 51, .name = "pale_green", .rgb = {184, 248, 184, 255} }
#define COLOR_NES_MAGIC_MINT (struct ColorPaletteEntry){ .index = 52, .name = "magic_mint", .rgb = {184, 248, 216, 255} }
#define COLOR_NES_CYAN (struct ColorPaletteEntry){ .index = 53, .name = "cyan", .rgb = {0, 252, 252, 255} }
#define COLOR_NES_CAROUSEL_PINK (struct ColorPaletteEntry){ .index = 54, .name = "carousel_pink", .rgb = {248, 216, 248, 255} }
#define NUM_NES_COLORS 55

extern struct ColorPaletteEntry global_color_palette_NES[NUM_NES_COLORS];

void color_copy(const Color c, Color r);
void color_round(const Vec4f a, Color r);
void color_lerp(const Color a, const Color b, float t, Color r);
void color_random(Color r);

#endif
