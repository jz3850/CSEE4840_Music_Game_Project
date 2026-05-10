/* vga_framebuffer.h
 *
 * Yangyang Zhang
 * Columbia University CSEE 4840 - Embedded Systems
 *
 * Adapted from code by Stephen A. Edwards, Columbia University
 */

#ifndef _VGA_FRAMEBUFFER_H
#define _VGA_FRAMEBUFFER_H

#include <linux/ioctl.h>
#ifdef __KERNEL__
#include <linux/io.h>
#else
#include <stdint.h>
#endif

typedef struct {
  uint32_t addr;    // 0..307199
  uint8_t  color;   // 0..255
} vga_framebuffer_pixel_t;

#define VGA_FRAMEBUFFER_MAGIC 'q'

/* ioctls and their arguments */
#define VGA_FRAMEBUFFER_WRITE_PIXEL _IOW(VGA_FRAMEBUFFER_MAGIC, 1, vga_framebuffer_pixel_t)

#endif
