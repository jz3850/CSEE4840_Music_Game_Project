#include "fbputchar.h"
#include "usbkeyboard.h"
#include "Note.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "sprite.h"
#include "framebuffer.h"
#include "audio_user.h"
#include <time.h>

// framebuffer_init("/dev/vga_framebuffer"); // initialize framebuffer
// sprite_init(); // initialize and load sprites

// sprite_exit(); // release sprites
// framebuffer_exit(); // release framebuffer

// framebuffer_clear(0);
// sprite_draw(sprite_0, 100, 100, 0, 0);
// fbputs(10, 20, "Hello, FPGA!", 1);
// fbputchar(10, 10, 'A', 1);
// framebuffer_swap();

// Keyboard
struct libusb_device_handle *keyboard;
uint8_t endpoint_address;
// Menu Items
#define MENU_ITEMS 4

//--------------------------------------------------Menu--------------------------------------------------------------------------
const char *menu_options[MENU_ITEMS] = {
    "Play",
    "Tutorial",
    "Difficulty",
    "Exit"};
void display_menu(int selected_index)
{
    framebuffer_clear(0);
    sprite_draw(sprite_logo, 80, 0, 0, 0);
    fbputs(200, 300, "Welcome to Rhythm Master!", 1);
    fbputs(200, 320, "Please select an option:", 1);

    // Display menu options
    for (int i = 0; i < MENU_ITEMS; i++)
    {
        if (i == selected_index)
        {
            fbputs(190, 360 + i * 20, "> ", 1);
            fbputs(200, 360 + i * 20, menu_options[i], 1);
        }
        else
        {
            fbputs(190, 360 + i * 20, "  ", 1);
            fbputs(200, 360 + i * 20, menu_options[i], 1);
        }
    }
    framebuffer_swap();
}

void tutorial()
{
    framebuffer_clear(0);
    fbputs(50, 70, "Welcome to Rhythm Master!", 1);
    fbputs(50, 100, "This is a rhythm-based music game.", 1);
    fbputs(50, 130, "You need to press keys in synchronous with falling notes.", 1);
    fbputs(50, 160, "There are 6 paths, and you can eliminate the falling notes by", 1);
    fbputs(50, 190, "pressing a s d j k l when they enter determination area.", 1);
    fbputs(50, 220, "Early or late press results in loss of points.", 1);
    fbputs(50, 250, "Miss press earns no point.", 1);
    fbputs(50, 280, "Combo earns extra points.", 1);
    fbputs(50, 310, "While playing, you can pause the game by pressing enter.", 1);
    fbputs(50, 340, "When game completed, you'll ge given a score and a level.", 1);
    fbputs(50, 370, "Press Enter to return to the main menu...", 1);
    framebuffer_swap();
    struct usb_keyboard_packet packet;
    int transferred;
    while (1)
    {
        libusb_interrupt_transfer(keyboard, endpoint_address, (unsigned char *)&packet, sizeof(packet), &transferred, 0);
        if (transferred == sizeof(packet))
        {
            if (packet.keycode[0] == 0x28)
            {          // Enter key
                break; // Exit the loop and return to the main menu
            }
        }
    }
}

#define DIFFICULTY_ITEMS 3
const char *difficulty_options[DIFFICULTY_ITEMS] = {
    "Easy",
    "Medium",
    "Hard"};
// int note_interval = 5;
int current_difficulty = 0;
void difficulty()
{
    int selected = current_difficulty;
    struct usb_keyboard_packet packet;
    int transferred;
    while (1)
    {
        framebuffer_clear(0);
        for (int i = 0; i < DIFFICULTY_ITEMS; i++)
        {
            if (i == selected)
            {
                fbputs(230, 200 + i * 30, "> ", 1);
            }
            else
            {
                fbputs(230, 200 + i * 30, "  ", 1);
            }
            fbputs(240, 200 + i * 30, difficulty_options[i], 1);
        }
        framebuffer_swap();
        libusb_interrupt_transfer(keyboard, endpoint_address, (unsigned char *)&packet, sizeof(packet), &transferred, 0);
        if (transferred == sizeof(packet))
        {
            if (packet.keycode[0] == 0x52)
            { // Up
                selected = (selected - 1 + DIFFICULTY_ITEMS) % DIFFICULTY_ITEMS;
            }
            else if (packet.keycode[0] == 0x51)
            { // Down
                selected = (selected + 1) % DIFFICULTY_ITEMS;
            }
            else if (packet.keycode[0] == 0x28)
            { // Enter
                current_difficulty = selected;
                break;
            }
        }
        usleep(100000);
    }
}

#define MUSIC_ITEMS 2
const char *music_options[MUSIC_ITEMS] = {
    "Littlestar",
    "CanonRock"};
int music_selected = 0;
void choose_music()
{
    int selected = music_selected;
    struct usb_keyboard_packet packet;
    int transferred;
    while (1)
    {
        framebuffer_clear(0);
        for (int i = 0; i < MUSIC_ITEMS; i++)
        {
            if (i == selected)
            {
                fbputs(230, 200 + i * 30, "> ", 1);
            }
            else
            {
                fbputs(230, 200 + i * 30, "  ", 1);
            }
            fbputs(240, 200 + i * 30, music_options[i], 1);
        }
        framebuffer_swap();
        libusb_interrupt_transfer(keyboard, endpoint_address, (unsigned char *)&packet, sizeof(packet), &transferred, 0);
        if (transferred == sizeof(packet))
        {
            if (packet.keycode[0] == 0x52)
            { // Up
                selected = (selected - 1 + MUSIC_ITEMS) % MUSIC_ITEMS;
            }
            else if (packet.keycode[0] == 0x51)
            { // Down
                selected = (selected + 1) % MUSIC_ITEMS;
            }
            else if (packet.keycode[0] == 0x28)
            { // Enter
                music_selected = selected;
                break;
            }
        }
        usleep(100000);
    }
}
//--------------------------------------------------Play--------------------------------------------------------------------------

int note_sequence[] = {1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6, 6, 5, 4, 3, 2, 1, 1, 1, 1, 1};

#define MAX_ACTIVE_NOTES 1000
#define PAUSE_MENU_ITEMS 3
const char *pause_menu_options[PAUSE_MENU_ITEMS] = {
    "Continue",
    "Restart",
    "Back to Main Menu"};

typedef struct
{
    int track;
    int frame;
    int active;
    // long created_sys_time;
    // int created_audio_time;
} Note;

typedef struct
{
    int show;
    int timer;
} TrackFeedback;

void draw_pause_menu(int selected_index, int combo, int score)
{

    fbputs(280, 170, "PAUSED", 1);

    char combo_str[32], score_str[32];
    sprintf(combo_str, "Combo: %d", combo);
    sprintf(score_str, "Score: %d", score);
    fbputs(180, 200, combo_str, 1);
    fbputs(180, 220, score_str, 1);

    for (int i = 0; i < PAUSE_MENU_ITEMS; i++)
    {
        if (i == selected_index)
        {
            fbputs(190, 250 + i * 30, "> ", 1);
            fbputs(200, 250 + i * 30, pause_menu_options[i], 1);
        }
        else
        {
            fbputs(190, 250 + i * 30, "  ", 1);
            fbputs(200, 250 + i * 30, pause_menu_options[i], 1);
        }
    }
}

int show_pause_menu(int *combo, int *score)
{
    int selected_index = 0;
    struct usb_keyboard_packet packet;
    int transferred;

    while (1)
    {
        framebuffer_clear(0);
        draw_pause_menu(selected_index, *combo, *score);
        framebuffer_swap();

        libusb_interrupt_transfer(keyboard, endpoint_address,
                                  (unsigned char *)&packet, sizeof(packet),
                                  &transferred, 10);

        if (transferred == sizeof(packet))
        {
            if (packet.keycode[0] == 0x28)
            {                          // Enter
                return selected_index; 
            }
            else if (packet.keycode[0] == 0x52)
            { // Up
                selected_index = (selected_index - 1 + PAUSE_MENU_ITEMS) % PAUSE_MENU_ITEMS;
            }
            else if (packet.keycode[0] == 0x51)
            { // Down
                selected_index = (selected_index + 1) % PAUSE_MENU_ITEMS;
            }
        }

        usleep(10000);
    }
}
void countdown_before_continue(int combo, int score, Note *active_notes, int active_count,
                               NoteAnimation **track_animations)
{
    for (int i = 3; i > 0; i--)
    {
        char count_str[32];
        sprintf(count_str, "Resuming in %d...", i);

        framebuffer_clear(0);
        draw_bg(combo, score);

        // redraw all notes before pause
        for (int j = 0; j < active_count; j++)
        {
            if (active_notes[j].active)
            {
                Note *note = &active_notes[j];
                NoteAnimation *anim = track_animations[note->track];
                int *frame = anim->frames[note->frame];
                sprite_t *sprites[6] = {sprite_note1, sprite_note2, sprite_note3, sprite_note4, sprite_note5, sprite_note6};
                sprite_draw(sprites[note->track], frame[0], frame[1], frame[2], frame[3]);
            }
        }

        // countdown
        fbputs(240, 200, count_str, 1);
        framebuffer_swap();
        sleep(1);
    }
}

int play()
{
    framebuffer_clear(0);
    const char *loading_str = "Loading...";
    fbputs(240, 200, loading_str, 1);
    framebuffer_swap();
    Note active_notes[MAX_ACTIVE_NOTES] = {{0}};
    int active_count = 0;
    // int current_note_index = 0;
    // int note_generation_timer = 0;
    // int note_generation_interval = 100;
    int game_over = 0;
    TrackFeedback track_feedbacks[6] = {{0}};
    int combo = 0;
    int bonus = 0;
    int great_count = 0;
    int perfect_count = 0;
    int score = 0;

    NoteAnimation *track_animations[] = {&POS1, &POS2, &POS3, &POS4, &POS5, &POS6};

    struct usb_keyboard_packet packet;
    int transferred;

    struct timespec last_tick, now;
    clock_gettime(CLOCK_MONOTONIC, &last_tick);
    int64_t pause_total_ms = 0;

    char audio_file[32];
    char note_file[32];

    snprintf(audio_file, sizeof(audio_file), "./music/%s.raw", music_options[music_selected]);

    if (load_audio(audio_file) < 0)
    {
        fprintf(stderr, "Failed to loading audio.\n");
        return 0;
    }

    if (current_difficulty == 0)
    {
        snprintf(note_file, sizeof(note_file), "./music/%s_easy.map", music_options[music_selected]);
    }
    else
    {
        snprintf(note_file, sizeof(note_file), "./music/%s.map", music_options[music_selected]);
    }
    if (load_note_file(note_file) < 0)
    {
        fprintf(stderr, "Failed to loading notes.\n");
        return 0;
    }

    int current_event_index = 0;
    int music_started = 0;
    double difficulty_drop_time_ms = (current_difficulty == 2) ? 5000.0 : 8000.0;
    double frame_time_ms = difficulty_drop_time_ms / NOTE_ANIMATION;
    const int drop_time_ms = (int)(frame_time_ms * NOTE_ANIMATION + 0.5);
    struct timespec sys_start;
    clock_gettime(CLOCK_MONOTONIC, &sys_start);
    int64_t time_offset = 0;
    reset_audio();
    // play_audio(0);

    while (!game_over)
    {
        // int ms = audio_time();
        // printf("ms: %d\n", ms);
        // check pause
        libusb_interrupt_transfer(keyboard, endpoint_address,
                                  (unsigned char *)&packet, sizeof(packet),
                                  &transferred, 3);

        if (transferred == sizeof(packet) && packet.keycode[0] == 0x28)
        { // press Enter to pause
            int ms = 0;
            struct timespec pause_start;
            if (music_started) 
            {
                ms = pulse_audio();
            }
            else
            {
                clock_gettime(CLOCK_MONOTONIC, &pause_start);
            }
            int pause_result = show_pause_menu(&combo, &score);

            switch (pause_result)
            {
            case 0: // Continue
                countdown_before_continue(combo, score, active_notes, active_count,
                                          track_animations);
                if (music_started)
                {
                    play_audio(ms); // recover audio
                }
                else
                {
                    struct timespec pause_end;
                    clock_gettime(CLOCK_MONOTONIC, &pause_end);

                    int64_t pause_delta_ms =
                        (pause_end.tv_sec - pause_start.tv_sec) * 1000 +
                        (pause_end.tv_nsec - pause_start.tv_nsec) / 1000000;

                    pause_total_ms += pause_delta_ms;
                }
                break;
            case 1: // Restart
                memset(active_notes, 0, sizeof(active_notes));
                active_count = 0;
                // current_note_index = 0;
                // note_generation_timer = 0;
                memset(track_feedbacks, 0, sizeof(track_feedbacks));
                combo = 0;
                great_count = 0;
                perfect_count = 0;
                score = 0;
                music_started = 0; // restart audio playing
                reset_audio();     
                time_offset = 0;
                clock_gettime(CLOCK_MONOTONIC, &sys_start);
                current_event_index = 0;
                break;
            case 2:           // Back to Main Menu
                free_audio(); // stop audio
                unload_note_file();
                return 0;
            }
        }

        // game logic
        if (transferred == sizeof(packet))
        {
            for (int i = 0; i < 6; i++)
            { // check each key
                uint8_t key = packet.keycode[i];
                if (key == 0)
                    continue;
                int track = -1;
                switch (key)
                {
                case 0x04:
                    track = 0;
                    break; // a
                case 0x16:
                    track = 1;
                    break; // s
                case 0x07:
                    track = 2;
                    break; // d
                case 0x0d:
                    track = 3;
                    break; // j
                case 0x0e:
                    track = 4;
                    break; // k
                case 0x0f:
                    track = 5;
                    break; // l
                default:
                    break;
                }
                if (track != -1)
                {
                    // find the bottom note
                    int max_frame = -1;
                    int selected_idx = -1;
                    for (int j = 0; j < active_count; j++)
                    {
                        if (active_notes[j].active && active_notes[j].track == track)
                        {
                            if (active_notes[j].frame > max_frame)
                            {
                                max_frame = active_notes[j].frame;
                                selected_idx = j;
                            }
                        }
                    }

                    if (selected_idx != -1)
                    {
                        Note *note = &active_notes[selected_idx];
                        NoteAnimation *anim = track_animations[note->track];
                        int y = anim->frames[note->frame][1];
                        if (y > 200)
                        {
                            note->active = 0; // eliminate note

                            if (y > 460)
                            { // Miss (>460)
                                combo = 0;
                                track_feedbacks[track].show = -1;
                            }
                            else if (y < 340)
                            { // Great (200-340)
                                combo++;
                                if (combo > 10)
                                    bonus = 10;
                                else
                                    bonus = combo;
                                great_count++;
                                track_feedbacks[track].show = 1;
                                score = score + bonus * 5 + 30;
                            }
                            else
                            { // Perfect (340-460)
                                combo++;
                                if (combo > 10)
                                    bonus = 10;
                                else
                                    bonus = combo;
                                perfect_count++;
                                track_feedbacks[track].show = 2;
                                score = score + bonus * 5 + 50;
                            }
                            track_feedbacks[track].timer = 10;
                        }
                    }
                }
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &now);
        long elapsed_ms = (now.tv_sec - last_tick.tv_sec) * 1000 + (now.tv_nsec - last_tick.tv_nsec) / 1000000;

        if (elapsed_ms < frame_time_ms)
            continue; // wait for next frame

        last_tick = now; // update time

        // Update Notes
        for (int i = 0; i < active_count; i++)
        { // Miss
            if (active_notes[i].active)
            {
                active_notes[i].frame++;

                if (active_notes[i].frame >= NOTE_ANIMATION)
                { 
                    struct timespec t;
                    clock_gettime(CLOCK_MONOTONIC, &t);

                    combo = 0;
                    track_feedbacks[active_notes[i].track].show = -1;
                    track_feedbacks[active_notes[i].track].timer = 10;
                    active_notes[i].active = 0;
                }
            }
        }

        // spawn new notes
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        int64_t sys_elapsed =
            (now.tv_sec - sys_start.tv_sec) * 1000 +
            (now.tv_nsec - sys_start.tv_nsec) / 1000000 -
            pause_total_ms;

        int64_t ref_time;
        if (!music_started)
        {
            // audio not started, use system time
            time_offset = sys_elapsed;
            if (time_offset > drop_time_ms - (current_difficulty == 2 ? -200 : 400))
            {
                play_audio(0);
                music_started = 1;
            }
        }

        ref_time = time_offset + audio_time();

        while (current_event_index < note_event_count &&
               ref_time >= note_events[current_event_index].time_ms)
        {
            int tr = note_events[current_event_index].track;
            if (tr >= 0 && tr < 6 && active_count < MAX_ACTIVE_NOTES)
            {
                active_notes[active_count++] = (Note){
                    .track = tr,
                    .frame = 0,
                    .active = 1,
                    // .created_sys_time   = sys_elapsed,
                    // .created_audio_time = (int)ref_time
                };
                // printf("[NOTE #%d] Target time: %d ms, ref_time: %d ms\n",
                //       current_event_index, note_events[current_event_index].time_ms, (int)ref_time);
            }
            current_event_index++;
        }

        // update feedback display timer
        for (int i = 0; i < 6; i++)
        {
            if (track_feedbacks[i].timer > 0 && --track_feedbacks[i].timer == 0)
            {
                track_feedbacks[i].show = 0;
            }
        }

        // begin drawing
        framebuffer_clear(0);
        draw_bg(combo, score);

        // draw notes
        for (int i = 0; i < active_count; i++)
        {
            if (active_notes[i].active)
            {
                Note *note = &active_notes[i];
                NoteAnimation *anim = track_animations[note->track];
                int *frame = anim->frames[note->frame];
                sprite_t *sprites[6] = {sprite_note1, sprite_note2, sprite_note3, sprite_note4, sprite_note5, sprite_note6};
                sprite_draw(sprites[note->track], frame[0], frame[1], frame[2], frame[3]);
            }
        }

        // draw feedback
        for (int i = 0; i < 6; i++)
        {
            if (track_feedbacks[i].show)
            {
                sprite_t *sprite = track_feedbacks[i].show == -1 ? sprite_miss : track_feedbacks[i].show == 1 ? sprite_great
                                                                                                              : sprite_perfect;
                sprite_draw(sprite, i * 100, 430, 0, 0);
            }
        }

        framebuffer_swap();

        // check if game end
        if (is_audio_finished())
        {
            int all_inactive = 1;
            for (int i = 0; i < active_count; i++)
            {
                if (active_notes[i].active)
                {
                    all_inactive = 0;
                    break;
                }
            }
            game_over = all_inactive;
        }
    }

    // game over interface
    const int max_score = note_event_count * 100 - 225;
    float ratio = (float)score / max_score;
    framebuffer_clear(0);
    draw_score_final(score);
    if (ratio >= 0.90)
        sprite_draw(sprite_S, 240, 250, 0, 0);
    else if (ratio >= 0.80)
        sprite_draw(sprite_A, 250, 250, 0, 0);
    else if (ratio >= 0.60)
        sprite_draw(sprite_B, 250, 250, 0, 0);
    else if (ratio >= 0.40)
        sprite_draw(sprite_C, 250, 250, 0, 0);
    else
        sprite_draw(sprite_D, 250, 250, 0, 0);
    framebuffer_swap();

    // wait for Enter
    while (1)
    {
        libusb_interrupt_transfer(keyboard, endpoint_address, (unsigned char *)&packet, sizeof(packet), &transferred, 0);
        if (transferred == sizeof(packet) && packet.keycode[0] == 0x28)
            break;
        usleep(10000);
    }

    free_audio(); 
    unload_note_file();

    return 0;
#undef MAX_ACTIVE_NOTES
}

//------------------------------------------------------main---------------------------------------------------------
int main()
{
    framebuffer_init("/dev/vga_framebuffer"); // initialize framebuffer
    sprite_init();                            // initialize and load sprites

    struct usb_keyboard_packet packet;
    int transferred;
    // char keystate[12];

    // Open the keyboard
    if ((keyboard = openkeyboard(&endpoint_address)) == NULL)
    {
        fprintf(stderr, "Did not find a keyboard\n");
        exit(1);
    }
    // Variables for menu interaction
    int selected_index = 0; // Currently selected menu item
    int in_menu = 1;
    display_menu(selected_index);
    // Look for and handle keypresses
    for (;;)
    {
        libusb_interrupt_transfer(keyboard, endpoint_address, (unsigned char *)&packet, sizeof(packet), &transferred, 0);
        if (!in_menu || transferred == sizeof(packet))
        {
            if (!in_menu || packet.keycode[0] == 0x28)
            { // Break from submenu and back to main || Enter key
                if (!in_menu)
                { // Enter the menu
                    in_menu = 1;
                    display_menu(selected_index);
                }
                else
                { // Confirm the selected option
                    in_menu = 0;
                    switch (selected_index)
                    {
                    case 0: // Play
                        choose_music();
                        sleep(1);
                        play();
                        break;
                    case 1: // Tutorial
                        tutorial();
                        break;
                    case 2: // Difficulty
                        difficulty();
                        break;
                    case 3: // Exit
                        framebuffer_clear(0);
                        fbputs(250, 200, "Exiting...", 1);
                        framebuffer_swap();
                        sleep(2);
                        framebuffer_clear(0);
                        framebuffer_swap();
                        return 0;
                    }
                }
            }
            else if (packet.keycode[0] == 0x52)
            { // Up arrow pressed (keycode 0x52)
                if (in_menu)
                {
                    selected_index = (selected_index - 1 + MENU_ITEMS) % MENU_ITEMS;
                    display_menu(selected_index);
                }
            }
            else if (packet.keycode[0] == 0x51)
            { // Down arrow pressed (keycode 0x51)
                if (in_menu)
                {
                    selected_index = (selected_index + 1) % MENU_ITEMS;
                    display_menu(selected_index);
                }
            }
        }
    }
    sprite_exit();      // release sprites
    framebuffer_exit(); // release framebuffer
}