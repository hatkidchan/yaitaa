#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "args.h"

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

  return 0;
}
