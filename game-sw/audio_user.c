#include "audio_user.h"
#include "audio.h"
#include <stdint.h>     // for int16_t, uint8_t
#include <stdio.h>      // for FILE, fopen, fread, fclose, perror
#include <stdlib.h>     // for malloc, free
#include <string.h>     // for memset if needed
#include <unistd.h>     // for usleep, close, etc.
#include <fcntl.h>      // for open, O_WRONLY
#include <sys/ioctl.h>  // for ioctl
#include <pthread.h>    // for pthreads
#include <stdatomic.h>  // for atomic variables

#define DEVICE       "/dev/audio"
#define SAMPLE_RATE  48000
#define FRAME_BYTES  4  // 2 bytes for left + 2 bytes for right
#define CHUNK_SIZE   512

typedef struct {
    uint8_t *data;
    size_t size;
    size_t offset;
    size_t sample_rate;
    size_t frame_bytes;
} audio_buffer_t;

static audio_buffer_t audio_buf = {0};

static pthread_t audio_thread;
static atomic_int playing = 0;              // Atomic flag: whether audio is playing
static atomic_size_t play_start_ms = 0;     // Start time in ms
static atomic_size_t play_current_ms = 0;   // Current playback position in ms
static atomic_int audio_finished = 0;

int load_audio(const char *filename) {
    // printf("Loading audio file: %s\n", filename);
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Failed to open audio file");
        return -1;
    }

    fseek(f, 0, SEEK_END);
    audio_buf.size = ftell(f);
    rewind(f);

    audio_buf.data = malloc(audio_buf.size);
    if (!audio_buf.data) {
        perror("Memory allocation failed");
        fclose(f);
        return -1;
    }

    fread(audio_buf.data, 1, audio_buf.size, f);
    fclose(f);

    audio_buf.offset = 0;
    audio_buf.sample_rate = SAMPLE_RATE;
    audio_buf.frame_bytes = FRAME_BYTES;

    return 0;
}

void* play_thread_func(void *arg) {
    size_t offset_ms = atomic_load(&play_start_ms);
    size_t offset_bytes = (offset_ms * SAMPLE_RATE / 1000) * FRAME_BYTES;

    int fd_audio = open(DEVICE, O_WRONLY);
    if (fd_audio < 0) {
        perror("Failed to open /dev/audio");
        return NULL;
    }

    double ms_acc = offset_ms;
    size_t pos = offset_bytes;

    const size_t chunk_samples = 64; // 64 stereo samples (128 FIFO words)
    const size_t chunk_bytes = chunk_samples * FRAME_BYTES;

    while (pos < audio_buf.size && atomic_load(&playing)) {
        // Check if FIFO is ready
        int status;
        if (ioctl(fd_audio, AUDIO_READ_STATUS, &status) < 0) {
            perror("AUDIO_READ_STATUS failed");
            break;
        }

        int ws_l = (status >> 24) & 0xFF;
        int ws_r = (status >> 16) & 0xFF;
        int available_pairs = ws_l < ws_r ? ws_l : ws_r;

        if (available_pairs < 80) {
            usleep(200); // Wait a bit before trying again (non-busy wait)
            continue;
        }

        size_t remaining = audio_buf.size - pos;
        size_t to_write = remaining < chunk_bytes ? remaining : chunk_bytes;

        ssize_t written = write(fd_audio, audio_buf.data + pos, to_write);
        if (written <= 0) {
            perror("Audio write failed");
            break;
        }

        pos += written;

        double ms_written = (1000.0 * written) / (SAMPLE_RATE * FRAME_BYTES);
        ms_acc += ms_written;
        atomic_store(&play_current_ms, (size_t)ms_acc);

        usleep(1000);  // Control stream frequency: check roughly every 1ms
    }

    close(fd_audio);
    atomic_store(&playing, 0);

    if (pos >= audio_buf.size) {
        atomic_store(&audio_finished, 1);
    } else {
        atomic_store(&audio_finished, 0);
    }
    return NULL;
}

int play_audio(int time_ms) {
    if (!audio_buf.data) return -1;

    atomic_store(&play_start_ms, time_ms);
    atomic_store(&play_current_ms, time_ms);
    atomic_store(&audio_finished, 0);
    atomic_store(&playing, 1);

    if (pthread_create(&audio_thread, NULL, play_thread_func, NULL) != 0) {
        perror("Failed to create playback thread");
        return -1;
    }

    pthread_detach(audio_thread);  // Automatically release resources
    return 0;
}

int pulse_audio(void) {
    atomic_store(&audio_finished, 0);
    atomic_store(&playing, 0);
    return atomic_load(&play_current_ms);
}

int reset_audio(void) {
    atomic_store(&play_start_ms, 0);
    atomic_store(&play_current_ms, 0);
    atomic_store(&audio_finished, 0);
    return 0;
}

int audio_time(void) {
    return atomic_load(&play_current_ms);
}

int is_audio_finished(void) {
    return atomic_load(&audio_finished);
}

void free_audio(void) {
    atomic_store(&playing, 0);
    atomic_store(&audio_finished, 0);
    atomic_store(&play_start_ms, 0);
    atomic_store(&play_current_ms, 0);
    if (audio_buf.data) {
        free(audio_buf.data);
        audio_buf.data = NULL;
        audio_buf.size = 0;
    }
}
