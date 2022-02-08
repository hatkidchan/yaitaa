#include <stdio.h>
#include <math.h>
#include "mod_charmap.h"
#include "image.h"
#include "colors.h"
#include "commons.h"
#include "fmt_strings.h"

void __chr_start_output(asc_state_t s);
void __chr_start_line(asc_state_t s, bool final);
void __chr_put_pixel(asc_state_t s, rgba8 fg, bool final);
void __chr_putc_bw(asc_state_t s, int cc, bool final);
void __chr_putc_ansi(asc_state_t s, int fg, int cc, bool final);
void __chr_putc_256(asc_state_t s, int fg, int cc, bool final);
void __chr_putc_true(asc_state_t s, rgba8 fg, int cc, bool final);
void __chr_end_line(asc_state_t s, bool final);
void __chr_end_output(asc_state_t s);

typedef struct {
  int n_codepoints;
  int *codepoints;
} __m_cmap_userdata_t;

bool mod_cmap_prepare(asc_state_t *state)
{
  static __m_cmap_userdata_t userdata;
  static int codepoints[M_CMAP_MAX_CODEPOINTS];
  userdata.codepoints = codepoints;
  int n_cp = 0, cp, proc;
  char *charset = state->args.charset;
  while ((cp = get_codepoint(charset, &proc)) != 0)
  {  codepoints[n_cp++] = cp; charset += proc; }
  if (n_cp > M_CMAP_MAX_CODEPOINTS)
  {
    fprintf(stderr, "Error: maximum of %d codepoints is allowed, got %d\n",
        M_CMAP_MAX_CODEPOINTS, n_cp);
    return false;
  }
  userdata.n_codepoints = n_cp;
  state->userdata = (void *)&userdata;

  int width, height;
  get_size_keep_aspect(
      state->source_image->width * 2, state->source_image->height,
      state->args.width, state->args.height, &width, &height);
  LOG("Source size: %dx%d", state->source_image->width,
      state->source_image->height);
  LOG("Requested size: %dx%d", state->args.width, state->args.height);
  LOG("Resizing image to %dx%d", width, height);
  state->image = image_resize(state->source_image, width, height);
  if (state->args.dither)
    m_prepare_dither(state);
  return true;
}

bool mod_cmap_main(asc_state_t state)
{
  image_t *img = state.image;
  __chr_start_output(state);
  for (int y = 0; y < img->height; y++)
  {
    bool final = y >= img->height;
    __chr_start_line(state, final);
    for (int x = 0; x < img->width; x++)
    {
      __chr_put_pixel(state, img->pixels[x + y * img->width], x >= img->width);
    }
    __chr_end_line(state, final);
  }
  __chr_end_output(state);
  return true;
}

void __chr_start_output(asc_state_t state)
{
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, S_JSON_HEAD,
        state.image->width, state.image->height / 2);
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_HEAD);
}

void __chr_start_line(asc_state_t state, bool final)
{
  (void)final;
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, S_JSON_LSTA);
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_LSTA);
}

void __chr_put_pixel(asc_state_t s, rgba8 fg, bool final)
{
  float brightness = calc_brightness(fg);
  __m_cmap_userdata_t data = *(__m_cmap_userdata_t *)s.userdata;
  int ndx = floorf(brightness * data.n_codepoints);
  if (ndx >= data.n_codepoints) ndx = data.n_codepoints - 1;
  int codepoint = data.codepoints[ndx];
  switch (s.args.out_style)
  {
    case ASC_STL_BLACKWHITE:
      __chr_putc_bw(s, codepoint, final);
      break;
    case ASC_STL_ANSI_VGA:
    case ASC_STL_ANSI_XTERM:
    case ASC_STL_ANSI_DISCORD:
      __chr_putc_ansi(s, closest_color(*s.palette, fg), codepoint, final);
      break;
    case ASC_STL_256COLOR:
      __chr_putc_256(s, closest_color(c_palette_256, fg), codepoint, final);
      break;
    case ASC_STL_PALETTE:
      __chr_putc_true(s, clamp_to_pal(*s.palette, fg), codepoint, final);
      break;
    case ASC_STL_TRUECOLOR:
      __chr_putc_true(s, fg, codepoint, final);
      break;
    case ASC_STL_ENDL:
      break;
  }
}

void __chr_putc_bw(asc_state_t s, int cc, bool final)
{
  static char buf[8];
  set_codepoint(buf, cc);
  if (s.args.out_format == ASC_FMT_JSON)
  {
    fprintf(s.out_file, S_JSON_PUCC, cc, 0xFFFFFF);
    if (!final) fprintf(s.out_file, ",");
    fprintf(s.out_file, "\n");
  }
  else if (s.args.out_format == ASC_FMT_HTML)
    fprintf(s.out_file, S_HTML_PUBW, cc);
  else
    fprintf(s.out_file, "%s", buf);
}

void __chr_putc_ansi(asc_state_t s, int fg, int cc, bool final)
{
  static char buf[8];
  rgba8 fgc = s.palette->palette[fg];
  set_codepoint(buf, cc);
  if (s.args.out_format == ASC_FMT_JSON)
  {
    fprintf(s.out_file, S_JSON_PUCC, cc, RGBN(fgc));
    if (!final) fprintf(s.out_file, ",");
    fprintf(s.out_file, "\n");
  }
  else if (s.args.out_format == ASC_FMT_HTML)
    fprintf(s.out_file, S_HTML_PUCC, fgc.r, fgc.g, fgc.b, cc);
  else
    fprintf(s.out_file, S_ANSI_FG"%s", fg + (fg >= 8 ? 82 : 30), buf);
}

void __chr_putc_256(asc_state_t s, int fg, int cc, bool final)
{
  static char buf[8];
  rgba8 fgc = pal256_to_rgb(*s.palette, fg);
  set_codepoint(buf, cc);
  if (s.args.out_format == ASC_FMT_JSON)
  {
    fprintf(s.out_file, S_JSON_PUCC, cc, RGBN(fgc));
    if (!final) fprintf(s.out_file, ",");
    fprintf(s.out_file, "\n");
  }
  else if (s.args.out_format == ASC_FMT_HTML)
    fprintf(s.out_file, S_HTML_PUCC, fgc.r, fgc.g, fgc.b, cc);
  else
    fprintf(s.out_file, S_ANSI_FG_256"%s", fg, buf);
}

void __chr_putc_true(asc_state_t s, rgba8 fg, int cc, bool final)
{
  static char buf[8];
  set_codepoint(buf, cc);
  if (s.args.out_format == ASC_FMT_JSON)
  {
    fprintf(s.out_file, S_JSON_PUCC, cc, RGBN(fg));
    if (!final) fprintf(s.out_file, ",");
    fprintf(s.out_file, "\n");
  }
  else if (s.args.out_format == ASC_FMT_HTML)
    fprintf(s.out_file, S_HTML_PUCC, fg.r, fg.g, fg.b, cc);
  else
    fprintf(s.out_file, S_ANSI_FG_RGB"%s", fg.r, fg.g, fg.b, buf);
}

void __chr_end_line(asc_state_t state, bool final)
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

void __chr_end_output(asc_state_t state)
{
  if (state.args.out_format == ASC_FMT_JSON)
    fprintf(state.out_file, S_JSON_TAIL);
  else if (state.args.out_format == ASC_FMT_HTML)
    fprintf(state.out_file, S_HTML_TAIL);
}
