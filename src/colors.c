#include "colors.h"

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

int closest_color(palette_t *pal, rgba8 color)
{
  int nearest = -1;
  int32_t min_distance = 0x0fffffff;
  for (int i = 0; i < pal->n_colors; i++)
  {
    rgba8 pal_color = pal->palette[i];
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

void load_palette_gpl(palette_t *pal, FILE *fp)
{
  (void)pal; (void)fp;
  // TODO: load GNU palette file
}

void load_palette_raw(palette_t *pal, FILE *fp)
{
  (void)pal; (void)fp;
  // TODO: load raw palette file
}

void load_palette(palette_t *pal, FILE *fp)
{
  (void)pal; (void)fp;
  // TODO: guess palette file type and load it
}

