#ifndef _AUDIO_H
#define _AUDIO_H

#ifndef __KERNEL__
    #include <stdint.h>
#endif

#include <linux/ioctl.h>

#define AUDIO_MAGIC 'A'

typedef struct __attribute__((packed)){
    int16_t left;
    int16_t right;
} audio_sample_t;

#define AUDIO_WRITE_SAMPLE _IOW(AUDIO_MAGIC, 1, audio_sample_t)
#define AUDIO_READ_STATUS  _IOR(AUDIO_MAGIC, 2, int)

#endif