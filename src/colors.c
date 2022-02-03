#include "colors.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

palette_t c_palette_bw = {
  .n_colors = 2,
  .palette = {
    {   0,   0,   0, 0 },
    { 255, 255, 255, 0 }
  }
};

palette_t c_palette_256 = {
  .n_colors = 0
};

palette_t c_palette_ansi_discord = {
  .n_colors = 8,
  .palette = {
    { 0x4f, 0x54, 0x5c, 0 },
    { 0xd1, 0x31, 0x35, 0 },
    { 0x85, 0x99, 0x00, 0 },
    { 0xb5, 0x89, 0x00, 0 },
    { 0x26, 0x8b, 0xd2, 0 },
    { 0xd3, 0x36, 0x82, 0 },
    { 0xd3, 0x36, 0x82, 0 },
    { 0xff, 0xff, 0xff, 0 },
  }
};

palette_t c_palette_ansi_vga = {
  .n_colors = 16,
  .palette = {
    {   0,   0,   0, 0 },
    { 170,   0,   0, 0 },
    {   0, 170,   0, 0 },
    { 170,  85,   0, 0 },
    {   0,   0, 170, 0 },
    { 170,   0, 170, 0 },
    {   0, 170, 170, 0 },
    { 170, 170, 170, 0 },
    {  85,  85,  85, 0 },
    { 255,  85,  85, 0 },
    {  85, 255,  85, 0 },
    { 255, 255,  85, 0 },
    {  85,  85, 255, 0 },
    { 255,  85, 255, 0 },
    {  85, 255, 255, 0 },
    { 255, 255, 255, 0 }
  }
};

palette_t c_palette_ansi_xterm = {
  .n_colors = 16,
  .palette = {
    {   0,   0,   0, 0 },
    { 205,   0,   0, 0 },
    {   0, 205,   0, 0 },
    { 205, 205,   0, 0 },
    {   0,   0, 238, 0 },
    { 205,   0, 205, 0 },
    {   0, 205, 205, 0 },
    { 229, 229, 229, 0 },
    { 127, 127, 127, 0 },
    { 255,   0,   0, 0 },
    {   0, 255,   0, 0 },
    { 255, 255,   0, 0 },
    {   0,   0, 252, 0 },
    { 255,   0, 255, 0 },
    {   0, 255, 255, 0 },
    { 255, 255, 255, 0 },
  }
};

int color_difference(rgba8 a, rgba8 b)
{
  int16_t dr = (int16_t)a.r - (int16_t)b.r;
  int16_t dg = (int16_t)a.g - (int16_t)b.g;
  int16_t db = (int16_t)a.b - (int16_t)b.b;
  return dr * dr + dg * dg + db * db;
}

int closest_color(palette_t pal, rgba8 color)
{
  int nearest = -1;
  int32_t min_distance = 0x0fffffff;
  for (int i = 0; i < pal.n_colors; i++)
  {
    rgba8 pal_color = pal.palette[i];
    int32_t distance = color_difference(pal_color, color);
    if (distance < min_distance)
    {
      min_distance = distance;
      nearest = i;
    }
  }
  return nearest;
}

rgba8 pal256_to_rgb(palette_t pal, int ndx)
{
  (void)pal;
  rgba8 out = { 0, 0, 0, 255 };
  if (ndx >= 232)
  {
    int l = (ndx - 232) * 255 / 24;
    out.r = out.g = out.b = l;
  }
  else
  {
    ndx -= 16;
    out.b = (ndx % 6) * 42;
    ndx /= 6;
    out.g = (ndx % 6) * 42;
    ndx /= 6;
    out.r = (ndx % 6) * 42;
  }
  return out;      
}

void make_pal256(palette_t *dst, palette_t ansi)
{
  if (dst->n_colors == 256) return;
  dst->n_colors = 256;
  for (int i = 0; i < 256; i++)
  {
    rgba8 res = pal256_to_rgb(ansi, i);
    memcpy(&dst->palette[i], &res, sizeof(rgba8));
  }
}

float calc_brightness(rgba8 c)
{
  return 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;
}

bool load_palette_gpl(palette_t *pal, FILE *fp)
{
  static char buf[8192];
  fgets(buf, 8192, fp); // GIMP Palette
  fgets(buf, 8192, fp); // Name: %s
  fgets(buf, 8192, fp); // Columns: %d
  
  pal->n_colors = 0;
  while (!feof(fp) && pal->n_colors < 256)
  {
    fgets(buf, 8192, fp);
    int r, g, b;
    if (sscanf(buf, "%d %d %d", &r, &g, &b) == 3)
    {
      pal->palette[pal->n_colors].r = r;
      pal->palette[pal->n_colors].g = g;
      pal->palette[pal->n_colors].b = b;
      pal->n_colors++;
    }
  }
  return true;
}

bool load_palette_raw(palette_t *pal, FILE *fp)
{
  while (!feof(fp))
  {
    size_t sz = fread(&pal->palette[pal->n_colors++], 1, sizeof(rgba8), fp);
    if (sz == 0 && feof(fp)) break;
    if (sz != sizeof(rgba8)) return false;
  }
  return true;
}

bool load_palette(palette_t *pal, FILE *fp)
{
  static char head[16];
  if (fread(head, sizeof(char), 12, fp) < 12) return false;
  if (fseek(fp, 0, SEEK_SET) != 0) return false;
  if (!strncmp(head, "GIMP Palette", 12))
  {
    return load_palette_gpl(pal, fp);
  }
  else
  {
    return load_palette_raw(pal, fp);
  }
}

