#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "args.h"
#include "mod_blocks.h"
#include "mod_braille.h"

int main(int argc, char **argv)
{
  asc_args_t args;
  int res = parse_args(argc, argv, &args);
  if (res == 1) return 0;
  if (res < 0) return -res;
  
  asc_state_t state;
  res = prepare_state(argc, argv, args, &state);
  if (res == 1) return 0;
  if (res < 0) return -res;
  
  switch (args.mode)
  {
    case ASC_MOD_BLOCKS:
      mod_blocks_prepare(&state);
      mod_blocks_main(state);
      break;
    case ASC_MOD_BRAILLE:
      mod_braille_prepare(&state);
      mod_braille_main(state);
      break;
    default:
      break;
  }

  return 0;
}
