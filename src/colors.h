#ifndef _COLORS_H_
#define _COLORS_H_
#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint8_t r, g, b, a;
} rgba8;

typedef struct {
  int n_colors;
  rgba8 palette[255];
} palette_t;

extern palette_t c_palette_ansi_discord;
extern palette_t c_palette_ansi_vga;
extern palette_t c_palette_ansi_xterm;

int closest_color(palette_t *pal, rgba8 color);
void load_palette_gpl(palette_t *pal, FILE *fp);
void load_palette_raw(palette_t *pal, FILE *fp);
void load_palette(palette_t *pal, FILE *fp);

#endif
