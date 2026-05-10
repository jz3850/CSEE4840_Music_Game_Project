#ifndef _NOTE_H
#define _NOTE_H

#define NOTE_COUNT 25       
#define CHANNELS 6       

typedef struct {
    int time_ms;   // timestamp
    int track;     // track number 0～5
} NoteEvent;

extern NoteEvent *note_events;
extern int        note_event_count;

int load_note_file(const char *path);
int load_note_file_bin(const char *path);
void unload_note_file(void);

#define NOTE_ANIMATION 48
typedef struct {
    int frames[NOTE_ANIMATION][4];  // x, y, width, height
} NoteAnimation;

extern NoteAnimation POS1;
extern NoteAnimation POS2;
extern NoteAnimation POS3;
extern NoteAnimation POS4;
extern NoteAnimation POS5;
extern NoteAnimation POS6;

#endif
