#ifndef AUDIO_USER_H
#define AUDIO_USER_H

int load_audio(const char *filename);
int play_audio(int time_ms);
int pulse_audio(void);
int reset_audio(void);
int audio_time(void);
int is_audio_finished(void);
void free_audio(void);

#endif
