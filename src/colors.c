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

int closest_color(palette_t pal, rgba8 color)
{
  int nearest = -1;
  int32_t min_distance = 0x0fffffff;
  for (int i = 0; i < pal.n_colors; i++)
  {
    rgba8 pal_color = pal.palette[i];
    int16_t dr = pal_color.r - color.r;
    int16_t dg = pal_color.g - color.g;
    int16_t db = pal_color.b - color.b;
    int32_t distance = dr * dr + dg * dg + db * db;
    if (distance < min_distance)
    {
      min_distance = distance;
      nearest = i;
    }
  }
  return nearest;
}

int closest_256(palette_t pal, rgba8 color)
{
  (void)pal;
  if (color.r == color.g && color.g == color.b)
  {
    if (color.r < 8) return 16;
    if (color.r > 248) return 231;
    return 232 + (int)ceil((color.r - 8.0) / 247.0 * 24.0);
  }
  int oc = 16;
  oc += 36 * (int)ceil(color.r / 255.0 * 5.0);
  oc += 6  * (int)ceil(color.g / 255.0 * 5.0);
  oc +=      (int)ceil(color.b / 255.0 * 5.0);
  return oc;
}

rgba8 pal256_to_rgb(palette_t pal, int ndx)
{
  rgba8 out = { 0, 0, 0, 255 };
  if (ndx < 16)
  {
    return pal.palette[ndx];
  }
  else if (ndx >= 232)
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

