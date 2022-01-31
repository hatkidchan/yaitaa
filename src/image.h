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

image_t *image_load(FILE *file);
image_t *image_resize(image_t *img, int width, int height);
void image_unload(image_t *img);

void get_size_keep_aspect(int w, int h, int dw, int dh, int *ow, int *oh);

#endif
