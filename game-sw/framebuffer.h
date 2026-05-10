#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stdint.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

int framebuffer_init(const char *device);
void framebuffer_clear(uint8_t color_index);
void framebuffer_draw_sprite(const uint8_t *sprite_pixels, int sprite_width, int sprite_height, int dst_x, int dst_y);
void framebuffer_draw_pixel(int x, int y, uint8_t color);
void framebuffer_swap();  // swap next_buffer to now_buffer
void framebuffer_exit();

#endif

