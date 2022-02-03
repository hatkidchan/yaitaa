#ifndef _COLORS_H_
#define _COLORS_H_
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
  uint8_t r, g, b, a;
} rgba8;

typedef struct {
  int n_colors;
  rgba8 palette[256];
} palette_t;

extern palette_t c_palette_bw;
extern palette_t c_palette_256;
extern palette_t c_palette_ansi_discord;
extern palette_t c_palette_ansi_vga;
extern palette_t c_palette_ansi_xterm;

int closest_color(palette_t pal, rgba8 color);
rgba8 pal256_to_rgb(palette_t pal, int ndx);
void make_pal256(palette_t *dst, palette_t ansi);
bool load_palette_gpl(palette_t *pal, FILE *fp);
bool load_palette_raw(palette_t *pal, FILE *fp);
bool load_palette(palette_t *pal, FILE *fp);
float calc_brightness(rgba8 color);

#endif
