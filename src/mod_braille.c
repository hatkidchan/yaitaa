#include <stdio.h>
#include "mod_braille.h"
#include "image.h"
#include "colors.h"
#include "commons.h"
#include "fmt_strings.h"

int __bra_best_match_i(rgba8 a, rgba8 b, rgba8 t);
void __bra_putc_raw(asc_state_t s, uint8_t ch);
void __bra_putc_esc(asc_state_t s, uint8_t ch);
void __bra_start_output(asc_state_t s);
void __bra_start_line(asc_state_t s, bool final);
void __bra_put_pixel(asc_state_t s, rgba8 bg, rgba8 fg, uint8_t ch, bool final);
void __bra_putc_ansi(asc_state_t s, int bg, int fg, uint8_t ch, bool final);
void __bra_putc_256(asc_state_t s, int bg, int fg, uint8_t ch, bool final);
void __bra_putc_true(asc_state_t s, rgba8 bg, rgba8 fg, uint8_t ch, bool final);
void __bra_end_line(asc_state_t s, bool final);
void __bra_end_output(asc_state_t s);

void __bra_update2x4(image_t *img, rgba8 block[8], int x, int y)
{
  block[0] = img->pixels[(x + 0) + (y + 0) * img->width];
  block[3] = img->pixels[(x + 1) + (y + 0) * img->width];
  block[1] = img->pixels[(x + 0) + (y + 1) * img->width];
  block[4] = img->pixels[(x + 1) + (y + 1) * img->width];
  block[2] = img->pixels[(x + 0) + (y + 2) * img->width];
  block[5] = img->pixels[(x + 1) + (y + 2) * img->width];
  block[6] = img->pixels[(x + 0) + (y + 3) * img->width];
  block[7] = img->pixels[(x + 1) + (y + 3) * img->width];
}


bool mod_braille_prepare(asc_state_t *state)
{
  int width, height;
  get_size_keep_aspect(
      state->source_image->width, state->source_image->height,
      state->args.width * 2, state->args.height * 4, &width, &height);
  LOG("Source size: %dx%d", state->source_image->width,
      state->source_image->height);
  LOG("Requested size: %dx%d", state->args.width * 2, state->args.height * 4);
  width = (width / 2) * 2; height = (height / 4) * 4;
  LOG("Resizing image to %dx%d", width, height);
  state->image = image_resize(state->source_image, width, height);
  if (state->args.dither)
    m_prepare_dither(state);
  return true;
}

bool mod_braille_main(asc_state_t state)
{
  image_t *img = state.image;
  
  rgba8 bright, dark, block[8];
  int dist_max, dist_min, dist;
  
  __bra_start_output(state);
  for (int y = 0; y < img->height; y += 4)
  {
    bool final = y >= (img->height - 4);
    __bra_start_line(state, final);
    for (int x = 0; x < img->width; x += 2)
    {
      __bra_update2x4(img, block, x, y);

      bright = dark = block[0];
      dist_max = 0; dist_min = 0xFFFFFF;
      for (int i = 0; i < 8; i++)
      {
        dist = color_difference(block[i], PURE_BLACK);
        if (dist < dist_min)
        {
          dist_min = dist;
          dark = block[i];
        }
        if (dist > dist_max)
        {
          dist_max = dist;
          bright = block[i];
        }
      }
      
      uint8_t pixel = 0x00;
      for (int i = 0; i < 8; i++)
      {
        if (__bra_best_match_i(dark, bright, block[i]) != 0)
        {
          pixel |= (1 << i);
        }
      }
      __bra_put_pixel(state, dark, bright, pixel, x >= (img->width - 2));
    }
    __bra_end_line(state, final);
  }
  __bra_end_output(state);
  return true;
}

int __bra_best_match_i(rgba8 a, rgba8 b, rgba8 t)
{
  return color_difference(a, t) < color_difference(b, t) ? 0 : 1;
}

void __bra_putc_raw(asc_state_t state, uint8_t ch)
{
  int ccode = 0x2800 | ch;
  fputc(0xe2, state.out_file);
  fputc(0x80 | ((ccode >> 6) & 0x3f), state.out_file);
  fputc(0x80 | ((ccode >> 0) & 0x3f), state.out_file);
}

void __bra_start_output(asc_state_t state)
{
  int w = state.image->width / 2, h = state.image->height / 4;
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, S_JSON_HEAD, w, h);
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_HEAD);
}

void __bra_start_line(asc_state_t state, bool final)
{
  (void)final;
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, S_JSON_LSTA);
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_LSTA);
}

void __bra_put_pixel(asc_state_t s, rgba8 min, rgba8 max, uint8_t ch, bool fin)
{
  switch (s.args.out_style)
  {
    case ASC_STL_ANSI_VGA:
    case ASC_STL_ANSI_XTERM:
    case ASC_STL_ANSI_DISCORD:
      {
        palette_t pal = *get_palette_by_id(s.args.out_style);
        __bra_putc_ansi(s,
            closest_color(pal, min), closest_color(pal, max), ch, fin);
      }
      break;
    case ASC_STL_256COLOR:
      __bra_putc_256(s, closest_color(c_palette_256, min),
          closest_color(c_palette_256, max), ch, fin);
      break;
    case ASC_STL_TRUECOLOR:
      __bra_putc_true(s, min, max, ch, fin);
      break;
    case ASC_STL_PALETTE:
      __bra_putc_true(s, clamp_to_pal(*s.palette, min),
          clamp_to_pal(*s.palette, max), ch, fin);
      break;
    case ASC_STL_BLACKWHITE:
    case ASC_STL_ENDL:
      break;
  }
}

void __bra_putc_ansi(asc_state_t s, int bgi, int fgi, uint8_t ch, bool fin)
{
  rgba8 bg = s.palette->palette[bgi], fg = s.palette->palette[fgi];
  FILE *fp = s.out_file;
  switch (s.args.out_format)
  {
    case ASC_FMT_JSON:
      fprintf(fp, S_JSON_PRGB, ch, RGBN(fg), RGBN(bg));
      if (!fin) fprintf(fp, ", ");
      break;
    case ASC_FMT_HTML:
      fprintf(fp, S_HTML_PCBR, fg.r, fg.g, fg.b,
          bg.r, bg.g, bg.b, 0x2800 | ch);
      break;
    case ASC_FMT_ANSI:
      fprintf(fp, S_ANSI, fgi + (fgi > 8 ? 82 : 30), bgi + (bgi > 8 ? 82 : 30));
      __bra_putc_raw(s, ch);
      break;
    case ASC_FMT_ENDL:
      break;
  }
}

void __bra_putc_256(asc_state_t s, int bgi, int fgi, uint8_t ch, bool final)
{
  rgba8 bg = c_palette_256.palette[bgi];
  rgba8 fg = c_palette_256.palette[fgi];
  FILE *fp = s.out_file;
  switch (s.args.out_format)
  {
    case ASC_FMT_JSON:
      fprintf(fp, S_JSON_PRGB, 0x2800 | ch, RGBN(fg), RGBN(bg));
      if (!final) fprintf(fp, ", ");
      break;
    case ASC_FMT_HTML:
      fprintf(fp, S_HTML_PCBR, fg.r, fg.g, fg.b, bg.r, bg.g, bg.b, 0x2800 | ch);
      break;
    case ASC_FMT_ANSI:
      fprintf(fp, S_ANSI_256, fgi, bgi);
      __bra_putc_raw(s, ch);
      break;
    case ASC_FMT_ENDL:
      break;
  }
}

void __bra_putc_true(asc_state_t s, rgba8 bg, rgba8 fg, uint8_t ch, bool fin)
{
  int max_int = fg.r << 16 | fg.g << 8 | fg.b;
  int min_int = bg.r << 16 | bg.g << 8 | bg.b;
  FILE *fp = s.out_file;
  if (s.args.out_format == ASC_FMT_JSON)
  {
    fprintf(fp, S_JSON_PRGB, 0x2800 | ch, max_int, min_int);
    if (!fin) fprintf(fp, ", ");
  }
  else if (s.args.out_format == ASC_FMT_HTML)
  {
    fprintf(fp, S_HTML_PCBR, fg.r, fg.g, fg.b, bg.r, bg.g, bg.b, 0x2800 | ch);
  }
  else
  {
    fprintf(fp, S_ANSI_RGB, fg.r, fg.g, fg.b, bg.r, bg.g, bg.b);
    __bra_putc_raw(s, ch);
  }
}

void __bra_end_line(asc_state_t state, bool final)
{
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, final ? S_JSON_LEND_FINAL : S_JSON_LEND);
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_LEND);
  else
  {
    if (state.args.out_style != ASC_STL_BLACKWHITE)
      fprintf(state.out_file, "\033[0m");
    fprintf(state.out_file, "\n");
  }
}

void __bra_end_output(asc_state_t state)
{
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, "  ]\n}");
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, "</table>\n");
}

