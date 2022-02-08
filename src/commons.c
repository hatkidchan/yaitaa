#include "colors.h"
#include "commons.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef DISABLE_LOGGING
bool b_logging = false;
#endif

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

#ifndef DISABLE_LOGGING
void _log(const char *fmt, ...)
{
  if (!b_logging) return;
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[INFO] ");
  vfprintf(stderr, fmt, args);
  fputc('\n', stderr);
  va_end(args);
}
#endif

void c_fatal(int code, const char *reason, ...)
{
  fprintf(stderr, "Error: ");
  va_list args;
  va_start(args, reason);
  vfprintf(stderr, reason, args);
  va_end(args);
  fprintf(stderr, "\n");
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

// From: https://github.com/raysan5/raylib/blob/e5ee69a/src/rtext.c#L1729
__attribute__((annotate("oclint:suppress[bitwise operator in conditional]")))
__attribute__((annotate("oclint:suppress[high cyclomatic complexity]")))
__attribute__((annotate("oclint:suppress[high npath complexity]")))
__attribute__((annotate("oclint:suppress[high ncss method]")))
__attribute__((annotate("oclint:suppress[long method]")))
int get_codepoint(char *text, int *processed)
{
  int code = 0;
  int octet = (unsigned char)(text[0]);
  *processed = 1;

  if (octet <= 0x7f)
  {
    code = text[0];
  }
  else if ((octet & 0xe0) == 0xc0)
  {
    unsigned char octet1 = text[1];
    if ((octet1 == '\0') || ((octet1 >> 6) != 2))
    { *processed = 2; return 0; }
    if ((octet >= 0xc2) && (octet <= 0xdf))
    {
      code = ((octet & 0x1f) << 6) | (octet1 & 0x3f);
      *processed = 2;
    }
  }
  else if ((octet & 0xf0) == 0xe0)
  {
    unsigned char octet1 = text[1];
    unsigned char octet2 = '\0';
    if ((octet1 == '\0') || ((octet1 >> 6) != 2))
    { *processed = 2; return 0; }
    octet2 = text[2];
    if ((octet2 == '\0') || ((octet2 >> 6) != 2))
    { *processed = 3; return 0; }
    if (((octet == 0xe0) && !((octet1 >= 0xa0) && (octet1 < 0xbf))) ||
        ((octet == 0xed) && !((octet1 >= 0x80) && (octet1 <= 0x9f))))
    { *processed = 2; return 0; }
    if ((octet >= 0xe0) && (octet <= 0xef))
    { code = ((octet & 0xf) << 12) | ((octet1 & 0x3f) << 6) | (octet2 & 0x3f);
      *processed = 3; }
  }
  else if ((octet & 0xf8) == 0xf0)
  {
    if (octet > 0xf4) return 0;
    unsigned char octet1 = text[1];
    unsigned char octet2 = '\0';
    unsigned char octet3 = '\0';
    if ((octet1 == '\0') || ((octet1 >> 6) != 2))
    { *processed = 2; return 0; }
    octet2 = text[2];
    if ((octet2 == '\0') || ((octet2 >> 6) != 2))
    { *processed = 3; return 0; }
    if ((octet3 == '\0') || ((octet3 >> 6) != 2))
    { *processed = 4; return 0; }
    if (((octet == 0xf0) && !((octet1 >= 0x90) && (octet1 <= 0xbf))) ||
        ((octet == 0xf4) && !((octet1 >= 0x80) && (octet1 <= 0x8f))))
    { *processed = 2; return 0; }
    if (octet >= 0xf0)
    {
      code = (((octet & 0x7) << 18) |
          ((octet1 & 0x3f) << 12) |
          ((octet2 & 0x3f) << 6) |
          (octet3 & 0x3f));
      *processed = 4;
    }
  }
  return code;
}

// From: https://gist.github.com/MightyPork/52eda3e5677b4b03524e40c9f0ab1da5
__attribute__((annotate("oclint:suppress[high ncss method]")))
int set_codepoint(char *buf, int codepoint)
{
  if (codepoint <= 0x7F)
  {
    *buf++ = (char)codepoint;
    *buf++ = '\0';
    return 1;
  }
  else if (codepoint <= 0x07FF)
  {
    *buf++ = (char)(((codepoint >> 6) & 0x1F) | 0xC0);
    *buf++ = (char)(((codepoint >> 0) & 0x3F) | 0x80);
    *buf++ = '\0';
    return 2;
  }
  else if (codepoint <= 0xFFFF)
  {
    *buf++ = (char)(((codepoint >> 12) & 0x0F) | 0xE0);
    *buf++ = (char)(((codepoint >>  6) & 0x3F) | 0x80);
    *buf++ = (char)(((codepoint >>  0) & 0x3F) | 0x80);
    *buf++ = '\0';
    return 3;
  }
  else if (codepoint <= 0x10FFFF)
  {
    *buf++ = (char)(((codepoint >> 18) & 0x07) | 0xF0);
    *buf++ = (char)(((codepoint >> 12) & 0x3F) | 0x80);
    *buf++ = (char)(((codepoint >>  6) & 0x3F) | 0x80);
    *buf++ = (char)(((codepoint >>  0) & 0x3F) | 0x80);
    *buf++ = '\0';
    return 4;
  }
  *buf++ = '\xEF';
  *buf++ = '\xBF';
  *buf++ = '\xBD';
  *buf++ = '\0';
  return 0;
}

int n_codepoints(char *text)
{
  int proc = 0, length = 0;
  while (*text != '\0')
  {
    if (get_codepoint(text, &proc) == 0)
      break;
    length++;
    text += proc;
  }
  return length;
}

