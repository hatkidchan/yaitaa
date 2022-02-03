#include <stdio.h>
#include "mod_braille.h"
#include "image.h"
#include "colors.h"
#include "commons.h"

int __bra_best_match_i(rgba8 a, rgba8 b, rgba8 t);
void __bra_putc_raw(asc_state_t state, uint8_t ch);
void __bra_putc_esc(asc_state_t state, uint8_t ch);
void __bra_start_output(asc_state_t state);
void __bra_start_line(asc_state_t state, bool final);
void __bra_put_pixel(asc_state_t sta, rgba8 min, rgba8 max, uint8_t ch, bool final);
void __bra_putc_ansi(asc_state_t state, int i_min, int i_max, uint8_t ch, palette_t pal, bool final);
void __bra_putc_256(asc_state_t state, int i_min, int i_max, uint8_t ch, bool final);
void __bra_putc_true(asc_state_t state, rgba8 min, rgba8 max, uint8_t ch, bool final);
void __bra_end_line(asc_state_t state, bool final);
void __bra_end_output(asc_state_t state);


void mod_braille_prepare(asc_state_t *state)
{
  int w, h;
  get_size_keep_aspect(
      state->source_image->width, state->source_image->height,
      state->args.width * 2, state->args.height * 4, &w, &h);
  w = (w / 2) * 2; h = (h / 4) * 4;
  state->image = image_resize(state->source_image, w, h);
  m_prepare_dither(state);
}

void mod_braille_main(asc_state_t state)
{
  image_t *img = state.image;
  
  uint8_t braille_char = 0x00;
  rgba8 pix2x4[8];
  
  rgba8 color_max, color_min;
  int dist_max, dist_min, dist_min_d = 0xffffff, dist;
  
  __bra_start_output(state);
  for (int y = 0; y < img->height; y += 4)
  {
    bool final = y >= (img->height - 4);
    __bra_start_line(state, final);
    for (int x = 0; x < img->width; x += 2)
    {
      pix2x4[0] = img->pixels[(x + 0) + (y + 0) * img->width];
      pix2x4[3] = img->pixels[(x + 1) + (y + 0) * img->width];
      pix2x4[1] = img->pixels[(x + 0) + (y + 1) * img->width];
      pix2x4[4] = img->pixels[(x + 1) + (y + 1) * img->width];
      pix2x4[2] = img->pixels[(x + 0) + (y + 2) * img->width];
      pix2x4[5] = img->pixels[(x + 1) + (y + 2) * img->width];
      pix2x4[6] = img->pixels[(x + 0) + (y + 3) * img->width];
      pix2x4[7] = img->pixels[(x + 1) + (y + 3) * img->width];
      color_max = color_min = pix2x4[0];

      dist_max = 0;
      dist_min = dist_min_d;
      for (int i = 0; i < 8; i++)
      {
        dist = color_difference(pix2x4[i], PURE_BLACK);
        if (dist < dist_min)
        {
          dist_min = dist;
          color_min = pix2x4[i];
        }
        if (dist > dist_max)
        {
          dist_max = dist;
          color_max = pix2x4[i];
        }
      }
      
      braille_char = 0x00;
      for (int i = 0; i < 8; i++)
      {
        if (__bra_best_match_i(color_min, color_max, pix2x4[i]) != 0)
        {
          braille_char |= (1 << i);
        }
      }
      __bra_put_pixel(state, color_min, color_max, braille_char, final);
    }
    __bra_end_line(state, final);
  }
  __bra_end_output(state);
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

void __bra_putc_esc(asc_state_t state, uint8_t ch)
{
  fprintf(state.out_file, "\\u28%02x", ch);
}

void __bra_start_output(asc_state_t state)
{
  switch (state.args.out_format)
  {
    case ASC_FMT_JSON:
      fprintf(state.out_file, "{\n");
      fprintf(state.out_file, "  \"width\": %d,\n", state.image->width);
      fprintf(state.out_file, "  \"height\": %d,\n", state.image->height);
      fprintf(state.out_file, "  \"data\": [");
      break;
    case ASC_FMT_HTML:
      fprintf(state.out_file, "<table style=\"border-collapse: collapse;\">\n");
      break;
    default:
      break;
  }
}

void __bra_start_line(asc_state_t state, bool final)
{
  (void)final;
  switch (state.args.out_format)
  {
    case ASC_FMT_JSON:
      fprintf(state.out_file, "    [\n");
      break;
    case ASC_FMT_HTML:
      fprintf(state.out_file, "<tr>");
    default:
      break;
  }
}

void __bra_put_pixel(asc_state_t state, rgba8 min, rgba8 max, uint8_t ch, bool final)
{
  switch (state.args.out_style)
  {
    case ASC_STL_ANSI_VGA:
    case ASC_STL_ANSI_XTERM:
    case ASC_STL_ANSI_DISCORD:
      {
        palette_t pal;
        switch (state.args.out_style)
        {
          case ASC_STL_ANSI_VGA: pal = c_palette_ansi_vga; break;
          case ASC_STL_ANSI_XTERM: pal = c_palette_ansi_xterm; break;
          case ASC_STL_ANSI_DISCORD: pal = c_palette_ansi_discord; break;
          default: c_fatal(9, "[UNREACH] Palette is unset"); return;
        }
        __bra_putc_ansi(state,
            closest_color(pal, min), closest_color(pal, max), ch, pal, final);
      }
      break;
    case ASC_STL_256COLOR:
      {
        __bra_putc_256(state, closest_color(c_palette_256, min),
            closest_color(c_palette_256, max), ch, final);
      }
      break;
    case ASC_STL_TRUECOLOR:
      __bra_putc_true(state, min, max, ch, final);
      break;
    case ASC_STL_PALETTE:
      {
        palette_t *pal = state.palette;
        __bra_putc_true(state, pal->palette[closest_color(*pal, min)],
            pal->palette[closest_color(*pal, max)], ch, final);
      }
      break;
    case ASC_STL_BLACKWHITE:
    case ASC_STL_ENDL:
      break;
  }
}

void __bra_putc_ansi(asc_state_t state, int i_min, int i_max, uint8_t ch, palette_t pal, bool final)
{
  rgba8 min_rgb = pal.palette[i_min], max_rgb = pal.palette[i_max];
  int min_int = min_rgb.r << 16 | min_rgb.g << 8 | min_rgb.b;
  int max_int = max_rgb.r << 16 | max_rgb.g << 8 | max_rgb.b;
  FILE *fp = state.out_file;
  switch (state.args.out_format)
  {
    case ASC_FMT_JSON:
      fprintf(fp, "{ \"char\": \"\\u28%d\", \"fg\": %d, \"bg\": %d }",
          ch, max_int, min_int);
      if (!final) fprintf(fp, ", ");
      break;
    case ASC_FMT_HTML:
      fprintf(fp, "<td style=\"color: rgb(%d, %d, %d); background: rgb(%d, %d, %d);\">&#%d;</td>",
          max_rgb.r, max_rgb.g, max_rgb.b, min_rgb.r, min_rgb.g, min_rgb.b,
          0x2800 | ch);
      break;
    case ASC_FMT_ANSI:
      fprintf(fp, "\033[%d;%dm", i_max + (i_max > 8 ? 82 : 30), i_min + (i_min > 8 ? 82 : 30));
      __bra_putc_raw(state, ch);
      break;
    case ASC_FMT_ENDL:
      break;
  }
}

void __bra_putc_256(asc_state_t state, int i_min, int i_max, uint8_t ch, bool final)
{
  rgba8 min_rgb = c_palette_256.palette[i_min];
  rgba8 max_rgb = c_palette_256.palette[i_max];
  int min_int = min_rgb.r << 16 | min_rgb.g << 8 | min_rgb.b;
  int max_int = max_rgb.r << 16 | max_rgb.g << 8 | max_rgb.b;
  FILE *fp = state.out_file;
  switch (state.args.out_format)
  {
    case ASC_FMT_JSON:
      fprintf(fp, "{ \"char\": \"\\u28%d\", \"fg\": %d, \"bg\": %d }",
          ch, max_int, min_int);
      if (!final) fprintf(fp, ", ");
      break;
    case ASC_FMT_HTML:
      fprintf(fp, "<td style=\"color: rgb(%d, %d, %d); background: rgb(%d, %d, %d);\">&#%d;</td>",
          max_rgb.r, max_rgb.g, max_rgb.b, min_rgb.r, min_rgb.g, min_rgb.b,
          0x2800 | ch);
      break;
    case ASC_FMT_ANSI:
      fprintf(fp, "\033[38;5;%d;48;5;%dm", i_max, i_min);
      __bra_putc_raw(state, ch);
      break;
    case ASC_FMT_ENDL:
      break;
  }
}

void __bra_putc_true(asc_state_t state, rgba8 min, rgba8 max, uint8_t ch, bool final)
{
  int max_int = max.r << 16 | max.g << 8 | max.b;
  int min_int = min.r << 16 | min.g << 8 | min.b;
  FILE *fp = state.out_file;
  switch (state.args.out_format)
  {
    case ASC_FMT_JSON:
      fprintf(fp, "{ \"char\": \"\\u%d\", \"fg\": %d, \"bg\": %d }",
          0x2800 | ch, max_int, min_int);
      if (!final) fprintf(fp, ", ");
      break;
    case ASC_FMT_HTML:
      fprintf(fp, "<td style=\"color: rgb(%d, %d, %d); background: rgb(%d, %d, %d);\">&#%d;</td>",
          max.r, max.g, max.b, min.r, min.g, min.b, 0x2800 | ch);
      break;
    default:
      fprintf(fp, "\033[38;2;%d;%d;%d;48;2;%d;%d;%dm",
          max.r, max.g, max.b, min.r, min.g, min.b);
      __bra_putc_raw(state, ch);
      break;
  }
}

void __bra_end_line(asc_state_t state, bool final)
{
  switch (state.args.out_format)
  {
    case ASC_FMT_JSON:
      fprintf(state.out_file, final ? "    ]\n" : "    ],\n");
      break;
    case ASC_FMT_HTML:
      fprintf(state.out_file, "</tr>\n");
      break;
    default:
      if (state.args.out_style != ASC_STL_BLACKWHITE)
        fprintf(state.out_file, "\033[0m");
      fprintf(state.out_file, "\n");
      break;
  }
}

void __bra_end_output(asc_state_t state)
{
  switch (state.args.out_format)
  {
    case ASC_FMT_JSON:
      fprintf(state.out_file, "  ]\n}");
      break;
    case ASC_FMT_HTML:
      fprintf(state.out_file, "</table>\n");
      break;
    default:
      break;
  }
}

