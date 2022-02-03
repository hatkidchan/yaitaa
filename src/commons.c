#include "colors.h"
#include "commons.h"
#include <stdio.h>
#include <stdlib.h>

void m_prepare_dither(asc_state_t *state)
{
  if (state->args.dither)
  {
    image_t *res = NULL;
    switch (state->args.out_style)
    {
      case ASC_STL_BLACKWHITE:
        res = image_dither(state->image, c_palette_bw);
        break;
      case ASC_STL_ANSI_VGA:
        res = image_dither(state->image, c_palette_ansi_vga);
        break;
      case ASC_STL_ANSI_XTERM:
        res = image_dither(state->image, c_palette_ansi_xterm);
        break;
      case ASC_STL_ANSI_DISCORD:
        res = image_dither(state->image, c_palette_ansi_discord);
        break;
      case ASC_STL_256COLOR:
        res = image_dither(state->image, c_palette_256); 
        break;
      case ASC_STL_PALETTE:
        res = image_dither(state->image, *state->palette);
        break;
      case ASC_STL_TRUECOLOR:
      case ASC_STL_ENDL:
        break;
    }
    if (res != NULL)
    {
      image_unload(state->image);
      state->image = res;
    }
  }
}

void c_fatal(int code, const char *reason)
{
  fprintf(stderr, "Error: %s\n", reason);
  exit(code);
}
