#include "math_color.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct ColorPaletteEntry global_color_palette_EGA[NUM_EGA_COLORS] = {
    COLOR_EGA_RED,
    COLOR_EGA_BLUE,
    COLOR_EGA_GREEN,
    COLOR_EGA_YELLOW,
    COLOR_EGA_CYAN,
    COLOR_EGA_MAGENTA
};

struct ColorPaletteEntry global_color_palette_NES[NUM_NES_COLORS] = {
    COLOR_NES_BOULDER,
    COLOR_NES_BLUE,
    COLOR_NES_DUKE_BLUE,
    COLOR_NES_PURPLE_HEART,
    COLOR_NES_FLIRT,
    COLOR_NES_CARMINE,
    COLOR_NES_MARIO,
    COLOR_NES_RED_BERRY,
    COLOR_NES_SADDLE_BROWN,
    COLOR_NES_JAPANESE_LAUREL,
    COLOR_NES_CAMARONE,
    COLOR_NES_CRUSOE,
    COLOR_NES_ASTRONAUT_BLUE,
    COLOR_NES_BLACK,
    COLOR_NES_SILVER,
    COLOR_NES_AZURE_RADIANCE,
    COLOR_NES_BLUE_RIBBON,
    COLOR_NES_ELECTRIC_VIOLET,
    COLOR_NES_PURPLE_PIZZAZZ,
    COLOR_NES_RAZZMATAZZ,
    COLOR_NES_SCARLET,
    COLOR_NES_CHRISTINE,
    COLOR_NES_PIRATE_GOLD,
    COLOR_NES_GREEN,
    COLOR_NES_ISLAMIC_GREEN,
    COLOR_NES_GREEN_HAZE,
    COLOR_NES_FIJI_ISLAND,
    COLOR_NES_SMOKE,
    COLOR_NES_DODGER_BLUE,
    COLOR_NES_CORNFLOWER_BLUE,
    COLOR_NES_LAVENDER_PURPLE,
    COLOR_NES_BLUSH_PINK,
    COLOR_NES_BRILLIANT_ROSE,
    COLOR_NES_BITTERSWEET,
    COLOR_NES_NEON_CARROT,
    COLOR_NES_SELECTIVE_YELLOW,
    COLOR_NES_LIME,
    COLOR_NES_MALACHITE,
    COLOR_NES_SPRING_GREEN,
    COLOR_NES_BRIGHT_TURQUOISE,
    COLOR_NES_BATTLESHIP,
    COLOR_NES_WHITE_ALABASTER,
    COLOR_NES_FRESH_AIR,
    COLOR_NES_PERFUME,
    COLOR_NES_MAUVE,
    COLOR_NES_LAVENDER_PINK,
    COLOR_NES_FLAMINGO_PINK,
    COLOR_NES_DESERT_SAND,
    COLOR_NES_PEACH_YELLOW,
    COLOR_NES_MELLOW_YELLOW,
    COLOR_NES_MIDORI,
    COLOR_NES_PALE_GREEN,
    COLOR_NES_MAGIC_MINT,
    COLOR_NES_CYAN,
    COLOR_NES_CAROUSEL_PINK
};
#pragma GCC diagnostic pop

void color_copy(const Color c, Color r) {
    r[0] = c[0];
    r[1] = c[1];
    r[2] = c[2];
    r[3] = c[3];
}

void color_round(const Vec4f a, Color r) {
    Vec4f s = {0};
    s[0] = 255.0f*a[0];
    s[1] = 255.0f*a[1];
    s[2] = 255.0f*a[2];
    s[3] = 255.0f*a[3];

    r[0] = (s[0] >= 0.0f) ? (uint8_t)(s[0]+0.5f) : (uint8_t)(s[0]-0.5f);
    r[1] = (s[0] >= 0.0f) ? (uint8_t)(s[1]+0.5f) : (uint8_t)(s[1]-0.5f);
    r[2] = (s[0] >= 0.0f) ? (uint8_t)(s[2]+0.5f) : (uint8_t)(s[2]-0.5f);
    r[3] = (s[0] >= 0.0f) ? (uint8_t)(s[3]+0.5f) : (uint8_t)(s[3]-0.5f);
}

void color_lerp(const Color a, const Color b, float t, Color r) {
    float w = 1.0f/255.0f;

    Vec4f fa = { a[0]*w, a[1]*w, a[2]*w, a[3]*w };
    Vec4f fb = { b[0]*w, b[1]*w, b[2]*w, b[3]*w };

    Vec4f fr = {0};
    fr[0] = fa[0]*t + (1.0f-t)*fb[0];
    fr[1] = fa[1]*t + (1.0f-t)*fb[1];
    fr[2] = fa[2]*t + (1.0f-t)*fb[2];
    fr[3] = fa[3]*t + (1.0f-t)*fb[3];

    color_round(fr, r);
}

void color_random(Color r) {
    r[0] = (uint8_t)(((double)rand()/(double)(RAND_MAX/255.0f)));
    r[1] = (uint8_t)(((double)rand()/(double)(RAND_MAX/255.0f)));
    r[2] = (uint8_t)(((double)rand()/(double)(RAND_MAX/255.0f)));
    r[3] = (uint8_t)(((double)rand()/(double)(RAND_MAX/255.0f)));
}
