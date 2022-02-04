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
#ifndef _COLORS_H_
#define _COLORS_H_
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define PURE_BLACK ((rgba8){ 0, 0, 0, 0 })
#define RGBN(c) (c.r << 16 | c.g << 8 | c.b)

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

int color_difference(rgba8 a, rgba8 b);
int closest_color(palette_t pal, rgba8 color);
rgba8 pal256_to_rgb(palette_t pal, int ndx);
rgba8 clamp_to_pal(palette_t pal, rgba8 color);
void make_pal256(palette_t *dst, palette_t ansi);
bool load_palette_gpl(palette_t *pal, FILE *fp);
bool load_palette_raw(palette_t *pal, FILE *fp);
bool load_palette(palette_t *pal, FILE *fp);
float calc_brightness(rgba8 color);

#endif
