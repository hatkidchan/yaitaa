#ifndef _IMAGE_H_
#define _IMAGE_H_
#include <stdio.h>
#include "colors.h"

typedef struct {
  int width;
  int height;
  int errno;
  rgba8 *pixels;
} image_t;

typedef rgba8 (*dither_quantizer_t)(rgba8 clr, void *param);

image_t *image_load(FILE *file);
image_t *image_resize(image_t *img, int width, int height);
image_t *image_dither(image_t *img, palette_t palette);
image_t *image_dither_fn(image_t *img, dither_quantizer_t quantize, void *p);
void image_unload(image_t *img);

void get_size_keep_aspect(int w, int h, int dw, int dh, int *ow, int *oh);

#endif
