#include "sprite.h"
#include "framebuffer.h"
#include "color.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

// Define all sprites
sprite_t *sprite_0 = NULL;
sprite_t *sprite_1 = NULL;
sprite_t *sprite_2 = NULL;
sprite_t *sprite_3 = NULL;
sprite_t *sprite_4 = NULL;
sprite_t *sprite_5 = NULL;
sprite_t *sprite_6 = NULL;
sprite_t *sprite_7 = NULL;
sprite_t *sprite_8 = NULL;
sprite_t *sprite_9 = NULL;
sprite_t *sprite_A = NULL;
sprite_t *sprite_B = NULL;
sprite_t *sprite_bg = NULL;
sprite_t *sprite_C = NULL;
sprite_t *sprite_combo = NULL;
sprite_t *sprite_D = NULL;
sprite_t *sprite_great = NULL;
sprite_t *sprite_logo = NULL;
sprite_t *sprite_miss = NULL;
sprite_t *sprite_note1 = NULL;
sprite_t *sprite_note2 = NULL;
sprite_t *sprite_note3 = NULL;
sprite_t *sprite_note4 = NULL;
sprite_t *sprite_note5 = NULL;
sprite_t *sprite_note6 = NULL;
sprite_t *sprite_notep1 = NULL;
sprite_t *sprite_notep2 = NULL;
sprite_t *sprite_notep3 = NULL;
sprite_t *sprite_notep4 = NULL;
sprite_t *sprite_notep5 = NULL;
sprite_t *sprite_notep6 = NULL;
sprite_t *sprite_perfect = NULL;
sprite_t *sprite_S = NULL;
sprite_t *sprite_score = NULL;

sprite_t *sprite_load(const char *filename) {
    // Use stb_image to load PNG (lightweight and simpler than libpng)
    extern unsigned char *stbi_load(const char *filename, int *x, int *y, int *channels_in_file, int desired_channels);
    extern void stbi_image_free(void *retval_from_stbi_load);

    int width, height, channels;
    unsigned char *img_data = stbi_load(filename, &width, &height, &channels, 4); // Force convert to RGBA

    if (!img_data) {
        printf("Failed to load image: %s\n", filename);
        return NULL;
    }

    sprite_t *sprite = (sprite_t *)malloc(sizeof(sprite_t));
    sprite->width = width;
    sprite->height = height;
    sprite->pixels = (uint8_t *)malloc(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 4;
            uint8_t r = img_data[idx + 0];
            uint8_t g = img_data[idx + 1];
            uint8_t b = img_data[idx + 2];
            uint8_t a = img_data[idx + 3];

            if (a == 0) {
                sprite->pixels[y * width + x] = 255;  // Transparent pixel
            } else {
                sprite->pixels[y * width + x] = color_find_index(r, g, b); // Map to palette color
            }
        }
    }

    stbi_image_free(img_data);
    return sprite;
}

void sprite_free(sprite_t *sprite) {
    if (sprite) {
        free(sprite->pixels);
        free(sprite);
    }
}

void sprite_init(void) {
    sprite_0 = sprite_load("./sprites/0.png");
    sprite_1 = sprite_load("./sprites/1.png");
    sprite_2 = sprite_load("./sprites/2.png");
    sprite_3 = sprite_load("./sprites/3.png");
    sprite_4 = sprite_load("./sprites/4.png");
    sprite_5 = sprite_load("./sprites/5.png");
    sprite_6 = sprite_load("./sprites/6.png");
    sprite_7 = sprite_load("./sprites/7.png");
    sprite_8 = sprite_load("./sprites/8.png");
    sprite_9 = sprite_load("./sprites/9.png");
    sprite_A = sprite_load("./sprites/A.png");
    sprite_B = sprite_load("./sprites/B.png");
    sprite_bg = sprite_load("./sprites/bg.png");
    sprite_C = sprite_load("./sprites/C.png");
    sprite_combo = sprite_load("./sprites/combo.png");
    sprite_D = sprite_load("./sprites/D.png");
    sprite_great = sprite_load("./sprites/great.png");
    sprite_logo = sprite_load("./sprites/logo.png");
    sprite_miss = sprite_load("./sprites/miss.png");
    sprite_note1 = sprite_load("./sprites/note1.png");
    sprite_note2 = sprite_load("./sprites/note2.png");
    sprite_note3 = sprite_load("./sprites/note3.png");
    sprite_note4 = sprite_load("./sprites/note4.png");
    sprite_note5 = sprite_load("./sprites/note5.png");
    sprite_note6 = sprite_load("./sprites/note6.png");
    sprite_notep1 = sprite_load("./sprites/notep1.png");
    sprite_notep2 = sprite_load("./sprites/notep2.png");
    sprite_notep3 = sprite_load("./sprites/notep3.png");
    sprite_notep4 = sprite_load("./sprites/notep4.png");
    sprite_notep5 = sprite_load("./sprites/notep5.png");
    sprite_notep6 = sprite_load("./sprites/notep6.png");
    sprite_perfect = sprite_load("./sprites/perfect.png");
    sprite_S = sprite_load("./sprites/S.png");
    sprite_score = sprite_load("./sprites/score.png");
}

void sprite_exit(void) {
    sprite_free(sprite_0);
    sprite_free(sprite_1);
    sprite_free(sprite_2);
    sprite_free(sprite_3);
    sprite_free(sprite_4);
    sprite_free(sprite_5);
    sprite_free(sprite_6);
    sprite_free(sprite_7);
    sprite_free(sprite_8);
    sprite_free(sprite_9);
    sprite_free(sprite_A);
    sprite_free(sprite_B);
    sprite_free(sprite_bg);
    sprite_free(sprite_C);
    sprite_free(sprite_combo);
    sprite_free(sprite_D);
    sprite_free(sprite_great);
    sprite_free(sprite_logo);
    sprite_free(sprite_miss);
    sprite_free(sprite_note1);
    sprite_free(sprite_note2);
    sprite_free(sprite_note3);
    sprite_free(sprite_note4);
    sprite_free(sprite_note5);
    sprite_free(sprite_note6);
    sprite_free(sprite_notep1);
    sprite_free(sprite_notep2);
    sprite_free(sprite_notep3);
    sprite_free(sprite_notep4);
    sprite_free(sprite_notep5);
    sprite_free(sprite_notep6);
    sprite_free(sprite_perfect);
    sprite_free(sprite_S);
    sprite_free(sprite_score);
}

void sprite_draw(sprite_t *sprite, int dst_x, int dst_y, int target_w, int target_h) {
    if (!sprite) return;

    int src_w = sprite->width;
    int src_h = sprite->height;

    if (target_w <= 0) target_w = src_w;
    if (target_h <= 0) target_h = src_h;

    for (int y = 0; y < target_h; y++) {
        for (int x = 0; x < target_w; x++) {
            int src_x = x * src_w / target_w;
            int src_y = y * src_h / target_h;

            uint8_t color = sprite->pixels[src_y * src_w + src_x];
            if (color == 255) continue;

            int draw_x = dst_x + x;
            int draw_y = dst_y + y;

            if (draw_x >= 0 && draw_x < 640 && draw_y >= 0 && draw_y < 480) {
                framebuffer_draw_pixel(draw_x, draw_y, color);
            }
        }
    }
}

void split_digits(int num, int digits[5]) {
    if (num < 0) num = -num;  
    digits[0] = (num / 10000) % 10;  // Ten-thousands place
    digits[1] = (num / 1000)  % 10;  // Thousands place
    digits[2] = (num / 100)   % 10;  // Hundreds place
    digits[3] = (num / 10)    % 10;  // Tens place
    digits[4] = num % 10;            // Units place
}

void draw_score(int score){
    sprite_draw(sprite_score, 380, 10, 80, 30);
    sprite_t *sprites[10] = {sprite_0, sprite_1, sprite_2, sprite_3, sprite_4,
                             sprite_5, sprite_6, sprite_7, sprite_8, sprite_9};
    int digits[5];
    split_digits(score, digits);
    for (int i = 0; i < 5; i++) {
        sprite_draw(sprites[digits[i]], 465 + i * 25, 10, 0, 0);
    }
}

void draw_score_final(int score){
    sprite_draw(sprite_score, 200, 200, 80, 30);
    sprite_t *sprites[10] = {sprite_0, sprite_1, sprite_2, sprite_3, sprite_4,
                             sprite_5, sprite_6, sprite_7, sprite_8, sprite_9};
    int digits[5];
    split_digits(score, digits);
    for (int i = 0; i < 5; i++) {
        sprite_draw(sprites[digits[i]], 290 + i * 25, 200, 0, 0);
    }
}

void draw_combo(int combo){
    sprite_draw(sprite_combo, 10, 10, 80, 30);
    sprite_t *sprites[10] = {sprite_0, sprite_1, sprite_2, sprite_3, sprite_4,
                             sprite_5, sprite_6, sprite_7, sprite_8, sprite_9};
    int digits[5];
    split_digits(combo, digits);
    for (int i = 0; i < 5; i++) {
        sprite_draw(sprites[digits[i]], 100 + i * 25, 10, 0, 0);
    }
}

void draw_bg (int combo, int score) {
    sprite_draw(sprite_bg, 0, 0, 600, 480);
    draw_combo(combo);
    draw_score(score);
}
