#include <stdio.h>
#include "mod_blocks.h"
#include "image.h"
#include "colors.h"
#include "commons.h"

const char *BLOCKS[4] = { " ", "\xe2\x96\x80", "\xe2\x96\x84", "\xe2\x96\x88" };
const char *BLOCKS_ESC[4] = { " ", "\\u2580", "\\u2584", "\\u2588" };


void mod_blocks_prepare(asc_state_t *state)
{
  int w, h;
  get_size_keep_aspect(
      state->source_image->width, state->source_image->height,
      state->args.width, state->args.height * 2, &w, &h);
  h = (h / 2) * 2;
  state->image = image_resize(state->source_image, w, h);
  m_prepare_dither(state);
}

void __blk_start_output(asc_state_t state)
{
  switch (state.args.out_format)
  {
    case ASC_FMT_JSON:
      fprintf(state.out_file, "{\n");
      fprintf(state.out_file, "  \"width\": %d,\n", state.image->width);
      fprintf(state.out_file, "  \"height\": %d,\n", state.image->height / 2);
      fprintf(state.out_file, "  \"data\": [");
      break;
    case ASC_FMT_HTML:
      fprintf(state.out_file, "<table style=\"border-collapse: collapse;\">\n");
      break;
    default:
      break;
  }
}

void __blk_end_output(asc_state_t state)
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

void __blk_start_line(FILE *fp, asc_format_t fmt, bool final)
{
  (void)final;
  switch (fmt)
  {
    case ASC_FMT_JSON:
      fprintf(fp, "    [\n");
      break;
    case ASC_FMT_HTML:
      fprintf(fp, "<tr>");
    default:
      break;
  }
}

void __blk_end_line(FILE *fp, asc_format_t fmt, asc_style_t stl, bool final)
{
  switch (fmt)
  {
    case ASC_FMT_JSON:
      fprintf(fp, final ? "    ]\n" : "    ],\n");
      break;
    case ASC_FMT_HTML:
      fprintf(fp, "</tr>\n");
      break;
    default:
      if (stl != ASC_STL_BLACKWHITE) fprintf(fp, "\033[0m");
      fprintf(fp, "\n");
      break;
  }
}

void __blk_putc_bw(int ndx, FILE *fp, asc_format_t fmt, bool final)
{
  switch (fmt)
  {
    case ASC_FMT_JSON:
      fprintf(fp, final ? "\"%s\"" : "\"%s\", ", BLOCKS_ESC[ndx]);
      break;
    case ASC_FMT_HTML:
      fprintf(fp, "<td>%s</td>", BLOCKS[ndx]);
      break;
    default:
      fprintf(fp, "%s", BLOCKS[ndx]);
      break;
  }
}

void __blk_putc_ansi(FILE *fp, asc_format_t fmt, bool final, int ct, int cb, palette_t pal)
{
  rgba8 top_rgb = pal.palette[ct], bot_rgb = pal.palette[cb];
  int top_int = top_rgb.r << 16 | top_rgb.g << 8 | top_rgb.b;
  int bot_int = bot_rgb.r << 16 | bot_rgb.g << 8 | bot_rgb.b;
  switch (fmt)
  {
    case ASC_FMT_JSON:
      fprintf(fp, "{ \"char\": \"%s\", \"fg\": %d, \"bg\": %d }",
          BLOCKS_ESC[1], top_int, bot_int);
      if (!final) fprintf(fp, ", ");
      break;
    case ASC_FMT_HTML:
      fprintf(fp, "<td style=\"color: rgb(%d, %d, %d); background: rgb(%d, %d, %d);\">%s</td>",
          top_rgb.r, top_rgb.g, top_rgb.b,
          bot_rgb.r, bot_rgb.g, bot_rgb.b,
          BLOCKS[1]);
      break;
    default:
      fprintf(fp, "\033[%d;%dm%s",
          ct + (ct >= 8 ? 82 : 30),
          cb + (cb >= 8 ? 92 : 40),
          BLOCKS[1]);
      break;
  }
}

void __blk_putc_256(FILE *fp, asc_format_t fmt, bool final, int ct, int cb, palette_t pal)
{
  rgba8 top_rgb = pal256_to_rgb(pal, ct), bot_rgb = pal256_to_rgb(pal, cb);
  int top_int = top_rgb.r << 16 | top_rgb.g << 8 | top_rgb.b;
  int bot_int = bot_rgb.r << 16 | bot_rgb.g << 8 | bot_rgb.b;
  switch (fmt)
  {
    case ASC_FMT_JSON:
      fprintf(fp, "{ \"char\": \"%s\", \"fg\": %d, \"bg\": %d }",
          BLOCKS_ESC[1], top_int, bot_int);
      if (!final) fprintf(fp, ", ");
      break;
    case ASC_FMT_HTML:
      fprintf(fp, "<td style=\"color: rgb(%d, %d, %d); background: rgb(%d, %d, %d);\">%s</td>",
          top_rgb.r, top_rgb.g, top_rgb.b,
          bot_rgb.r, bot_rgb.g, bot_rgb.b,
          BLOCKS[1]);
      break;
    default:
      fprintf(fp, "\033[38;5;%d;48;5;%dm%s", ct, cb, BLOCKS[1]);
      break;
  }
}

void __blk_putc_truecolor(FILE *fp, asc_format_t fmt, bool final, rgba8 top, rgba8 bot)
{
  int top_int = top.r << 16 | top.g << 8 | top.b;
  int bot_int = bot.r << 16 | bot.g << 8 | bot.b;
  switch (fmt)
  {
    case ASC_FMT_JSON:
      fprintf(fp, "{ \"char\": \"%s\", \"fg\": %d, \"bg\": %d }",
          BLOCKS_ESC[1], top_int, bot_int);
      if (!final) fprintf(fp, ", ");
      break;
    case ASC_FMT_HTML:
      fprintf(fp, "<td style=\"color: rgb(%d, %d, %d); background: rgb(%d, %d, %d);\">%s</td>",
          top.r, top.g, top.b, bot.r, bot.g, bot.b, BLOCKS[1]);
      break;
    default:
      fprintf(fp, "\033[38;2;%d;%d;%d;48;2;%d;%d;%dm%s",
          top.r, top.g, top.b, bot.r, bot.g, bot.b, BLOCKS[1]);
      break;
  }
}

void __blk_put_pixel(asc_state_t state, rgba8 top, rgba8 bot, bool final)
{
  asc_format_t fmt = state.args.out_format;
  FILE *fp = state.out_file;
  switch (state.args.out_style)
  {
    case ASC_STL_BLACKWHITE:
      {
        bool bri_top = calc_brightness(top) > 0.5;
        bool bri_bot = calc_brightness(bot) > 0.5;
        if ( bri_top &&  bri_bot) __blk_putc_bw(3, fp, fmt, final);
        if (!bri_top &&  bri_bot) __blk_putc_bw(2, fp, fmt, final);
        if ( bri_top && !bri_bot) __blk_putc_bw(1, fp, fmt, final);
        if (!bri_top && !bri_bot) __blk_putc_bw(0, fp, fmt, final);
      }
      break;
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
        int index_top = closest_color(pal, top),
            index_bot = closest_color(pal, bot);
        __blk_putc_ansi(fp, fmt, final, index_top, index_bot, pal);
      }
      break;
    case ASC_STL_256COLOR:
      {
        int index_top = closest_color(c_palette_256, top),
            index_bot = closest_color(c_palette_256, bot);
        __blk_putc_256(fp, fmt, final, index_top, index_bot, c_palette_ansi_vga);
      }
      break;
    case ASC_STL_TRUECOLOR:
      __blk_putc_truecolor(fp, fmt, final, top, bot);
      break;
    case ASC_STL_PALETTE:
      {
        palette_t *pal = state.palette;
        rgba8 pal_top = pal->palette[closest_color(*pal, top)];
        rgba8 pal_bot = pal->palette[closest_color(*pal, bot)];
        __blk_putc_truecolor(fp, fmt, final, pal_top, pal_bot);
      }
    case ASC_STL_ENDL:
      break;
  }
}

void mod_blocks_main(asc_state_t state)
{
  image_t *img = state.image;
  __blk_start_output(state);
  for (int y = 0; y < img->height; y += 2)
  {
    bool final = y >= (img->height - 2);
    __blk_start_line(state.out_file, state.args.out_format, final);
    for (int x = 0; x < img->width; x++)
    {
      rgba8 top = img->pixels[x + y * img->width];
      rgba8 bot = img->pixels[x + (y + 1) * img->width];
      __blk_put_pixel(state, top, bot, x >= (img->width - 1));
    }
    __blk_end_line(state.out_file, state.args.out_format, state.args.out_style, final);
  }
  __blk_end_output(state);
}
