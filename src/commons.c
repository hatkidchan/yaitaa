#include "colors.h"
#include "commons.h"
#include <stdio.h>
#include <stdlib.h>

void m_prepare_dither(asc_state_t *sta)
{
  image_t *res = NULL;
  switch (sta->args.out_style)
  {
    case ASC_STL_BLACKWHITE:
    case ASC_STL_ANSI_VGA:
    case ASC_STL_ANSI_XTERM:
    case ASC_STL_ANSI_DISCORD:
    case ASC_STL_256COLOR:
      res = image_dither(sta->image, *get_palette_by_id(sta->args.out_style)); 
      break;
    case ASC_STL_PALETTE:
      res = image_dither(sta->image, *sta->palette);
      break;
    case ASC_STL_TRUECOLOR:
    case ASC_STL_ENDL:
      return;
  }
  image_unload(sta->image);
  sta->image = res;
}

void c_fatal(int code, const char *reason)
{
  fprintf(stderr, "Error: %s\n", reason);
  exit(code);
}

palette_t *get_palette_by_id(asc_style_t stl)
{
  palette_t *pal = &c_palette_bw;
  switch (stl)
  {
    case ASC_STL_BLACKWHITE: pal = &c_palette_bw; break;
    case ASC_STL_ANSI_VGA: pal = &c_palette_ansi_vga; break;
    case ASC_STL_ANSI_XTERM: pal = &c_palette_ansi_xterm; break;
    case ASC_STL_ANSI_DISCORD: pal = &c_palette_ansi_discord; break;
    case ASC_STL_256COLOR: pal = &c_palette_256; break;
    default: c_fatal(9, "[UNREACH] Palette is unset"); break;
  }
  return pal;
}
