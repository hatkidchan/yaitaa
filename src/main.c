/*
 *  yaitaa - yet another image to ascii art converter
 *  Copyright (C) 2022  hatkidchan <hatkidchan at gmail dot com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
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
    case ASC_MOD_GRADIENT:
      fprintf(stderr, "Error: ASC_MOD_GRADIENT is not implemented yet\n");
      break;
    case ASC_MOD_BRUTEFORCE:
      fprintf(stderr, "Error: ASC_MOD_BRUTEFORCE is not implemented yet\n");
      break;
    case ASC_MOD_ENDL:
      break;
  }

  return 0;
}
