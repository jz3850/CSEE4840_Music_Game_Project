#include "framebuffer.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include "vga_framebuffer.h"

static int fb_fd = -1;
static uint8_t *next_buffer = NULL;
static uint8_t *now_buffer = NULL;

int framebuffer_init(const char *device) {
    fb_fd = open(device, O_RDWR);
    if (fb_fd < 0) {
        perror("open framebuffer");
        return -1;
    }

    next_buffer = malloc(SCREEN_SIZE);
    now_buffer = malloc(SCREEN_SIZE);
    if (!next_buffer || !now_buffer) {
        perror("malloc buffer");
        framebuffer_exit();
        return -1;
    }

    memset(next_buffer, 255, SCREEN_SIZE);
    memset(now_buffer, 255, SCREEN_SIZE);

    return 0;
}

void framebuffer_clear(uint8_t color_index) {
    if (next_buffer) {
        memset(next_buffer, color_index, SCREEN_SIZE);
    }
}

void framebuffer_draw_sprite(const uint8_t *sprite_pixels, int sprite_width, int sprite_height, int dst_x, int dst_y) {
    if (!next_buffer) return;

    for (int y = 0; y < sprite_height; y++) {
        for (int x = 0; x < sprite_width; x++) {
            uint8_t color = sprite_pixels[y * sprite_width + x];
            if (color == 255) continue;

            int draw_x = dst_x + x;
            int draw_y = dst_y + y;

            if (draw_x < 0 || draw_x >= SCREEN_WIDTH || draw_y < 0 || draw_y >= SCREEN_HEIGHT)
                continue;

            next_buffer[draw_y * SCREEN_WIDTH + draw_x] = color;
        }
    }
}

void framebuffer_draw_pixel(int x, int y, uint8_t color) {
    if (!next_buffer) return;
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;
    next_buffer[y * SCREEN_WIDTH + x] = color;
}

void framebuffer_swap() {
    if (now_buffer && next_buffer) {
        for (int addr = 0; addr < SCREEN_WIDTH * SCREEN_HEIGHT; addr++) {
            if (now_buffer[addr] != next_buffer[addr]) {
                vga_framebuffer_pixel_t pixel;
                pixel.addr = addr;
                pixel.color = next_buffer[addr];
                ioctl(fb_fd, VGA_FRAMEBUFFER_WRITE_PIXEL, &pixel);

                now_buffer[addr] = next_buffer[addr]; // now_buffer
            }
        }
    }
}

void framebuffer_exit() {
    if (next_buffer) free(next_buffer);
    if (now_buffer) free(now_buffer);
    if (fb_fd >= 0) close(fb_fd);

    next_buffer = NULL;
    now_buffer = NULL;
    fb_fd = -1;
}
