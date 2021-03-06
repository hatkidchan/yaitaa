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
#include "commons.h"

__attribute__((annotate("oclint:suppress[high npath complexity]")))
int main(int argc, char **argv)
{
  asc_args_t args;
  int res = parse_args(argc, argv, &args);
  if (res == 1) return 0;
  else if (res < 0) return -res;

  asc_state_t state;
  res = prepare_state(argc, argv, args, &state);
  if (res == 1) return 0;
  else if (res < 0) return -res;

  asc_handler_t handler = asc_handlers[args.mode];
  if (handler.prepare == NULL)
    c_fatal(12, "this mode is not implemented yet");

  if (!handler.prepare(&state))
    c_fatal(15, "handler.prepare() said a big no-no");

  if (!handler.main(state))
    c_fatal(16, "handler.main() said a big no-no");
  
  if (state.image != state.source_image)
    image_unload(state.image);
  image_unload(state.source_image);

  if (args.out_style == ASC_STL_PALETTE)
    free(state.palette);

  return 0;
}
