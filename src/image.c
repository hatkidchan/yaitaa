#include "commons.h"
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

__attribute__((annotate("oclint:suppress[high cyclomatic complexity]")))
void __dither_update_pixel(image_t *img, int x, int y, int err[3], float bias)
{
  if (x < 0 || x >= img->width || y < 0 || y >= img->height) return;
  int i = x + y * img->width;
  rgba8 pix = img->pixels[i];
  int dst[3] = { pix.r, pix.g, pix.b };
  img->pixels[i].r = CLAMP(0, dst[0] + (int)((float)err[0] * bias), 255);
  img->pixels[i].g = CLAMP(0, dst[1] + (int)((float)err[1] * bias), 255);
  img->pixels[i].b = CLAMP(0, dst[2] + (int)((float)err[2] * bias), 255);
}


image_t *image_dither_fn(image_t *img, dither_quantizer_t quantize, void *param)
{
  image_t *res = calloc(1, sizeof(image_t));
  int w = res->width = img->width;
  int h = res->height = img->height;
  res->pixels = calloc(img->width * img->height, sizeof(rgba8));
  memcpy(res->pixels, img->pixels, img->width * img->height * sizeof(rgba8));
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      rgba8 old = res->pixels[x + y * w];
      rgba8 new = quantize(old, param);
      res->pixels[x + y * w].r = new.r;
      res->pixels[x + y * w].g = new.g;
      res->pixels[x + y * w].b = new.b;
      int err[3] = {
        (int)old.r - (int)new.r,
        (int)old.g - (int)new.g, 
        (int)old.b - (int)new.b
      };
      __dither_update_pixel(res, x + 1, y    , err, 7.0f / 16.0f);
      __dither_update_pixel(res, x - 1, y + 1, err, 3.0f / 16.0f);
      __dither_update_pixel(res, x    , y + 1, err, 5.0f / 16.0f);
      __dither_update_pixel(res, x + 1, y + 1, err, 1.0f / 16.0f);
    }
  }
  return res;
}

rgba8 __image_quantize_pal(rgba8 clr, void *param)
{
  palette_t palette = *(palette_t *)param;
  int ndx = closest_color(palette, clr);
  return palette.palette[ndx];
}

image_t *image_dither(image_t *img, palette_t pal)
{
  return image_dither_fn(img, __image_quantize_pal, &pal);
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
    if (fabsf(ratio - (float)tmp_1 / dh) < fabsf(ratio - (float)tmp_2 / dh))
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

