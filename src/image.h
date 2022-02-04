/*
 *  yaitaa - yet another image to ascii art converter
 *  Copyright (C) 2022  hatkidchan <hatkidchan at gmail dot com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef _IMAGE_H_
#define _IMAGE_H_
#include <stdio.h>
#include "colors.h"

typedef struct {
  int width;
  int height;
  int err;
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
