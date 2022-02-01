#include "image.h"
#include "stb_image.h"
#include "stb_image_resize.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

image_t *image_load(FILE *file)
{
  image_t *img = calloc(1, sizeof(image_t));
  int n;
  img->pixels = (rgba8 *)stbi_load_from_file(file,
      &img->width, &img->height,
      &n, STBI_rgb_alpha);
  return img;
}


image_t *image_resize(image_t *img, int width, int height)
{
  image_t *res = calloc(1, sizeof(image_t));
  res->width = width;
  res->height = height;
  res->pixels = calloc(width * height, sizeof(rgba8));
  stbir_resize_uint8((const unsigned char *)img->pixels,
      img->width, img->height, 0,
      (unsigned char *)res->pixels,
      res->width, res->height, 0, STBI_rgb_alpha);
  return res;
}

void image_unload(image_t *img)
{
  free(img->pixels);
  free(img);
}

void get_size_keep_aspect(int w, int h, int dw, int dh, int *ow, int *oh)
{
  *ow = dw;
  *oh = dh;
  float ratio = (float)w / (float)h;
  float ratio_dst = (float)dw / (float)dh;
  int tmp_1, tmp_2;
  if (ratio_dst >= ratio)
  {
    tmp_1 = floor(dh * ratio);
    tmp_2 = ceil(dh * ratio);
    if (fabsf(ratio - (float)tmp_1 / dh) < fabsf(ratio - (float)tmp_w / dh))
      *ow = tmp_1 < 1 ? 1 : tmp_1;
    else
      *ow = tmp_2 < 1 ? 1 : tmp_2;
  }
  else
  {
    tmp_1 = floor(dw / ratio);
    tmp_2 = ceil(dw / ratio);
    if (tmp_2 == 0 ||
        fabs(ratio - (float)dw / tmp_1) < fabs(ratio - (float)dw / tmp_2))
      (*oh) = tmp_1 < 1 ? 1 : tmp_1;
    else
      (*oh) = tmp_2 < 1 ? 1 : tmp_2;
  }
}

