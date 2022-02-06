#include <stdio.h>
#include "mod_blocks.h"
#include "image.h"
#include "colors.h"
#include "commons.h"
#include "fmt_strings.h"

const char *BLK[4] = { " ", "\xe2\x96\x80", "\xe2\x96\x84", "\xe2\x96\x88" };
const char *BLKJ[4] = { " ", "\\u2580", "\\u2584", "\\u2588" };

void __blk_start_output(asc_state_t s);
void __blk_start_line(asc_state_t s, bool final);
void __blk_put_pixel(asc_state_t s, rgba8 top, rgba8 bot, bool final);
void __blk_putc_bw(asc_state_t s, int top, int bot, bool final);
void __blk_putc_ansi(asc_state_t s, int top, int bot, bool final);
void __blk_putc_256(asc_state_t s, int top, int bot, bool final);
void __blk_putc_true(asc_state_t s, rgba8 top, rgba8 bot, bool final);
void __blk_end_line(asc_state_t s, bool final);
void __blk_end_output(asc_state_t s);


void mod_blocks_prepare(asc_state_t *state)
{
  int width, height;
  get_size_keep_aspect(
      state->source_image->width, state->source_image->height,
      state->args.width, state->args.height * 2, &width, &height);
  LOG("Source size: %dx%d", state->source_image->width,
      state->source_image->height);
  LOG("Requested size: %dx%d", state->args.width, state->args.height * 2);
  height = (height / 2) * 2;
  LOG("Resizing image to %dx%d", width, height);
  state->image = image_resize(state->source_image, width, height);
  if (state->args.dither)
    m_prepare_dither(state);
}

void mod_blocks_main(asc_state_t state)
{
  image_t *img = state.image;
  __blk_start_output(state);
  for (int y = 0; y < img->height; y += 2)
  {
    bool final = y >= (img->height - 2);
    __blk_start_line(state, final);
    for (int x = 0; x < img->width; x++)
    {
      rgba8 top = img->pixels[x + y * img->width];
      rgba8 bot = img->pixels[x + (y + 1) * img->width];
      __blk_put_pixel(state, top, bot, x >= (img->width - 1));
    }
    __blk_end_line(state, final);
  }
  __blk_end_output(state);
}

void __blk_start_output(asc_state_t state)
{
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, S_JSON_HEAD,
        state.image->width, state.image->height / 2);
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_HEAD);
}

void __blk_end_output(asc_state_t state)
{
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, S_JSON_TAIL);
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_TAIL);
}

void __blk_start_line(asc_state_t state, bool final)
{
  (void)final;
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, S_JSON_LSTA);
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_LSTA);
}

void __blk_end_line(asc_state_t state, bool final)
{
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, final ? S_JSON_LEND_FINAL : S_JSON_LEND);
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_LEND);
  else
  {
    if (state.args.out_style != ASC_STL_BLACKWHITE)
      fprintf(state.out_file, S_ANSI_RST);
    fprintf(state.out_file, "\n");
  }
}

void __blk_putc_bw(asc_state_t state, int top, int bot, bool final)
{
  if (state.args.out_format == ASC_FMT_JSON)
  {
    fprintf(state.out_file, S_JSON_PBBW, BLKJ[top | (bot << 1)]);
    if (!final) fprintf(state.out_file, ",");
    fprintf(state.out_file, "\n");
  }
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_PBBW, BLK[top | (bot << 1)]);
  else
    fprintf(state.out_file, "%s", BLK[top | (bot << 1)]);
}

void __blk_putc_ansi(asc_state_t state, int ct, int cb, bool final)
{
  rgba8 top = state.palette->palette[ct], bot = state.palette->palette[cb];
  if (state.args.out_format == ASC_FMT_JSON)
  {
    fprintf(state.out_file, S_JSON_PBLK, BLKJ[1], RGBN(top), RGBN(bot));
    if (!final) fprintf(state.out_file, ",");
    fprintf(state.out_file, "\n");
  }
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_PBLK, top.r, top.g, top.b,
        bot.r, bot.g, bot.b, BLK[1]);
  else
    fprintf(state.out_file, S_ANSI"%s", ct + (ct >= 8 ? 82 : 30),
        cb + (cb >= 8 ? 92 : 40), BLK[1]);
}

void __blk_putc_256(asc_state_t s, int ct, int cb, bool final)
{
  rgba8 top = pal256_to_rgb(*s.palette, ct),
        bot = pal256_to_rgb(*s.palette, cb);
  FILE *fp = s.out_file;
  
  if (s.args.out_format == ASC_FMT_JSON)
  {
    fprintf(fp, S_JSON_PBLK, BLKJ[1], RGBN(top), RGBN(bot));
    if (!final) fprintf(fp, ",");
    fprintf(s.out_file, "\n");
  }
  else if (s.args.out_format == ASC_FMT_HTML)
    fprintf(fp, S_HTML_PBLK, top.r, top.g, top.b, bot.r, bot.g, bot.b, BLK[1]);
  else
    fprintf(fp, S_ANSI_256"%s", ct, cb, BLK[1]);
}

void __blk_putc_true(asc_state_t state, rgba8 top, rgba8 bot, bool final)
{
  if (state.args.out_format == ASC_FMT_JSON)
  {
    fprintf(state.out_file, S_JSON_PBLK, BLKJ[1], RGBN(top), RGBN(bot));
    if (!final) fprintf(state.out_file, ",");
    fprintf(state.out_file, "\n");
  }
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_PBLK, top.r, top.g, top.b,
        bot.r, bot.g, bot.b, BLK[1]);
  else
    fprintf(state.out_file, S_ANSI_RGB"%s", top.r, top.g, top.b,
        bot.r, bot.g, bot.b, BLK[1]);
}

void __blk_put_pixel(asc_state_t state, rgba8 top, rgba8 bot, bool final)
{
  switch (state.args.out_style)
  {
    case ASC_STL_BLACKWHITE:
      {
        bool btop = calc_brightness(top) > 0.5;
        bool bbot = calc_brightness(bot) > 0.5;
        __blk_putc_bw(state, btop, bbot, final);
      }
      break;
    case ASC_STL_ANSI_VGA:
    case ASC_STL_ANSI_XTERM:
    case ASC_STL_ANSI_DISCORD:
      {
        palette_t pal = *state.palette;
        int itop = closest_color(pal, top), ibot = closest_color(pal, bot);
        __blk_putc_ansi(state, itop, ibot, final);
      }
      break;
    case ASC_STL_256COLOR:
      {
        int itop = closest_color(c_palette_256, top),
            ibot = closest_color(c_palette_256, bot);
        __blk_putc_256(state, itop, ibot, final);
      }
      break;
    case ASC_STL_PALETTE:
      {
        palette_t pal = *state.palette;
        __blk_putc_true(state, clamp_to_pal(pal, top),
            clamp_to_pal(pal, bot), final);
      }
      break;
    case ASC_STL_TRUECOLOR:
      __blk_putc_true(state, top, bot, final);
      break;
    case ASC_STL_ENDL:
      break;
  }
}
