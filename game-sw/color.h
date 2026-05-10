#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

extern const uint8_t color_palette[256][3]; // 256 colors, each is {r,g,b}

int color_find_index(uint8_t r, uint8_t g, uint8_t b);

#endif
