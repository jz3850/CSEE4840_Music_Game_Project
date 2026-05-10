#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>

typedef struct sprite {
    int width;
    int height;
    uint8_t *pixels;
} sprite_t;

// Load a single sprite
sprite_t *sprite_load(const char *filename);
void sprite_free(sprite_t *sprite);

// Unified initialization and cleanup of all sprites
void sprite_init(void);
void sprite_exit(void);

// All sprites exposed for use in main
extern sprite_t *sprite_0;
extern sprite_t *sprite_1;
extern sprite_t *sprite_2;
extern sprite_t *sprite_3;
extern sprite_t *sprite_4;
extern sprite_t *sprite_5;
extern sprite_t *sprite_6;
extern sprite_t *sprite_7;
extern sprite_t *sprite_8;
extern sprite_t *sprite_9;
extern sprite_t *sprite_A;
extern sprite_t *sprite_B;
extern sprite_t *sprite_bg;
extern sprite_t *sprite_C;
extern sprite_t *sprite_combo;
extern sprite_t *sprite_D;
extern sprite_t *sprite_great;
extern sprite_t *sprite_logo;
extern sprite_t *sprite_miss;
extern sprite_t *sprite_note1;
extern sprite_t *sprite_note2;
extern sprite_t *sprite_note3;
extern sprite_t *sprite_note4;
extern sprite_t *sprite_note5;
extern sprite_t *sprite_note6;
extern sprite_t *sprite_notep1;
extern sprite_t *sprite_notep2;
extern sprite_t *sprite_notep3;
extern sprite_t *sprite_notep4;
extern sprite_t *sprite_notep5;
extern sprite_t *sprite_notep6;
extern sprite_t *sprite_perfect;
extern sprite_t *sprite_S;
extern sprite_t *sprite_score;

// Draw sprite, supports scaling
void sprite_draw(sprite_t *sprite, int dst_x, int dst_y, int target_w, int target_h);
void draw_bg(int combo, int score);
void draw_combo(int combo);
void draw_score(int score);
void draw_score_final(int score);
void split_digits(int num, int digits[5]);

#endif // SPRITE_H
