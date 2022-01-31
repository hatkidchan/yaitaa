#include "args.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
  int value;
  char *strings[8];
  char *description;
} __option_t;

int __find_value(const __option_t *options, char *option);
void __print_options(const __option_t *options);

const __option_t __mode_options[ASC_MOD_ENDL + 1] = {
  { ASC_MOD_BLOCKS,
    { "b", "blk", "blocks", NULL },
    "Box-drawing characters (\342\226\204) (default)" },
  { ASC_MOD_BRAILLE,
    { "r", "brl", "braille", NULL },
    "Braille characters (literally stolen from MineOS)" },
  { ASC_MOD_GRADIENT,
    { "g", "grd", "gradient", NULL },
    "Gradient of characters. No matching at all" },
  { ASC_MOD_BRUTEFORCE,
    { "f", "guess", "bruteforce", NULL },
    "Looking for best possible character" },
  { -1, { NULL }, NULL }
};

const __option_t __style_options[ASC_STL_ENDL + 1] = {
  { ASC_STL_BLACKWHITE,
    { "1", "bw", "black-white", "1bit", NULL },
    "1-bit black/white" },
  { ASC_STL_ANSI_VGA,
    { "vga", "ansi-vga", NULL },
    "VGA palette" },
  { ASC_STL_ANSI_XTERM,
    { "xterm", "ansi-xterm", NULL },
    "xTerm palette. A bit more rough, compared to VGA" },
  { ASC_STL_ANSI_DISCORD,
    { "discord", "ansi-discord", NULL },
    "Palette in Discord ANSI highlight" },
  { ASC_STL_256COLOR,
    { "256", "pal256", "8bit", NULL },
    "256-color palette (default)" },
  { ASC_STL_TRUECOLOR,
    { "true", "truecolor", "24bit", NULL },
    "24-bit RGB (TrueColor)" },
  { ASC_STL_PALETTE,
    { "pal", "palette", "custom", NULL },
    "Custom palette (specified via -P)" },
  { -1, { NULL }, NULL }
};

const __option_t __format_options[ASC_FMT_ENDL + 1] = {
  { ASC_FMT_ANSI,
    { "ansi", "raw", NULL },
    "Output, suitable for terminal (default)" },
  { ASC_FMT_HTML,
    { "html", NULL },
    "Output as HTML table" },
  { ASC_FMT_JSON,
    { "json", NULL },
    "Output as JSON 2D array of characters with properties" },
  { -1, { NULL }, NULL }
};

void usage(int argc, char **argv)
{
  (void)argc;
  fprintf(stderr, "usage: %s ", *argv);
  fprintf(stderr, "[-vhd] [-O FILE] [-W WIDTH] [-H HEIGHT] ");
  fprintf(stderr, "[-M MODE] [-S STYLE] [-F FORMAT] [-P PALETTE] ");
  fprintf(stderr, "FILE\n\n");
  fprintf(stderr, "-v\t\tEnable verbose mode\n");
  fprintf(stderr, "-h\t\tShow this help\n");
  fprintf(stderr, "-d\t\tEnable dithering\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "-O FILE\t\tOutput file. Default: - (stdout)\n");
  fprintf(stderr, "-W WIDTH\tOutput width (in characters)\n");
  fprintf(stderr, "-H HEIGHT\tOutput height (in characters)\n");
  fprintf(stderr, "-M MODE\t\tOutput mode\n");
  fprintf(stderr, "-S STYLE\tStyle (palette)\n");
  fprintf(stderr, "-F FORMAT\tOutput format\n");
  fprintf(stderr, "-P PALETTE\tPath to palette file (when -S pal)\n");
  fprintf(stderr, "\n\n");
  fprintf(stderr, "Options for MODE:\n");
  __print_options(__mode_options);
  fprintf(stderr, "Options for STYLE:\n");
  __print_options(__style_options);
  fprintf(stderr, "Options for FORMAT:\n");
  __print_options(__format_options);
}

int parse_args(int argc, char **argv, asc_args_t *args)
{
  args->input_filename = NULL;
  args->output_filename = "-";
  args->palette_filename = NULL;
  args->width = 80;
  args->height = 24;
  args->out_format = ASC_FMT_ANSI;
  args->out_style = ASC_STL_256COLOR;
  args->mode = ASC_MOD_BLOCKS;
  args->dither = false;
  args->verbose = false;
  args->charset = " .'-*+$@";
  int c;
  while ((c = getopt(argc, argv, "vhdW:H:M:S:F:P:O:")) != -1)
  {
    switch (c)
    {
      case 'v':
        args->verbose = true;
        break;
      case 'h':
        usage(argc, argv);
        return 1;
        break;
      case 'd':
        args->dither = true;
        break;
      case 'W':
        if ((args->width = atoi(optarg)) < 0)
        {
          fprintf(stderr, "Error: WIDTH is invalid\n");
          return -1;
        }
        break;
      case 'H':
        if ((args->height = atoi(optarg)) < 0)
        {
          fprintf(stderr, "Error: HEIGHT is invalid\n");
          return -1;
        }
        break;
      case 'M':
        {
          int val = __find_value(__mode_options, optarg);
          if (val < 0)
          {
            fprintf(stderr, "Error: invalid mode '%s'\n", optarg);
            return -1;
          }
          args->mode = val;
        }
        break;
      case 'S':
        {
          int val = __find_value(__style_options, optarg);
          if (val < 0)
          {
            fprintf(stderr, "Error: invalid style '%s'\n", optarg);
            return -1;
          }
          args->out_style = val;
        }
        break;
      case 'F':
        {
          int val = __find_value(__format_options, optarg);
          if (val < 0)
          {
            fprintf(stderr, "Error: invalid format '%s'\n", optarg);
            return -1;
          }
          args->out_format = val;
        }
        break;
      case 'P':
        args->palette_filename = optarg;
        break;
      case 'O':
        args->output_filename = optarg;
        break;
      case '?':
        if (optopt == 'O'
            || optopt == 'W' || optopt == 'H'
            || optopt == 'S' || optopt == 'M' || optopt == 'F')
        {
          fprintf(stderr, "Error: missing argument for -%c\n", optopt);
        }
        else
        {
          fprintf(stderr, "Error: Unknown parameter -%c\n", optopt);
        }
        return -2;
    }
  }
  if (args->out_style == ASC_STL_PALETTE && args->palette_filename == NULL)
  {
    fprintf(stderr, "Error: no palette file provided, but palette mode selected\n");
    return -3;
  }
  if (argc <= optind || argc < 2)
  {
    fprintf(stderr, "Error: no image provided\n");
    return -2;
  }
  args->input_filename = argv[optind];
  return 0;
}

int prepare_state(int argc, char **argv, asc_args_t args, asc_state_t *state)
{
  (void)argc; (void)argv;
  state->args = args;
  
  FILE *image_file;
  if ((image_file = fopen(args.input_filename, "rb")) == NULL
      || ferror(image_file) != 0)
  {
    int err = errno;
    fprintf(stderr, "Error: failed to open file %s for reading: %d: %s\n",
        args.input_filename, err, strerror(err));
    return -100 - err;
  }
  
  state->source_image = image_load(image_file);
  fclose(image_file);
  
  state->out_file = stdout;
  if (strcmp(args.output_filename, "-"))
    state->out_file = fopen(args.output_filename, "wb");
  if (state->out_file == NULL)
  {
    int err = errno;
    fprintf(stderr, "Error: failed to open file %s for writing: %d: %s\n",
            args.output_filename, err, strerror(err));
    return -100 - err;
  }
  return 0;
}

int __find_value(const __option_t *options, char *option)
{
  __option_t *opt;
  while ((opt = (__option_t *)options++)->value >= 0)
  {
    char *str = opt->strings[0];
    for (int i = 0; str != NULL; i++, str = opt->strings[i])
    {
      if (!strcmp(str, option)) return opt->value;
    }
  }
  return -1;
}

void __print_options(const __option_t *options)
{
  __option_t *opt;
  while ((opt = (__option_t *)options++)->value >= 0)
  {
    fprintf(stderr, " - %s:\n\t", opt->description);
    char *str = opt->strings[0];
    for (int i = 0; str != NULL; i++, str = opt->strings[i])
    {
      fprintf(stderr, "%s ", str);
    }
    fprintf(stderr, "\n");
  }
}
