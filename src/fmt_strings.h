#ifndef _FMT_STRINGS_H
#define _FMT_STRINGS_H

#define S_JSON_HEAD "{\n \"width\": %d,\n  \"height\": %d,\n  \"data\": [\n"
#define S_JSON_LSTA "    [\n"
#define S_JSON_PBBW "      { \"char\": \"%s\", \"fg\": 16777215, \"bg\": 0 }"
#define S_JSON_PRGB "      { \"char\": \"\\u%04x\", \"fg\": %d, \"bg\": %d }"
#define S_JSON_PBLK "      { \"char\": \"%s\", \"fg\": %d, \"bg\": %d }"
#define S_JSON_LEND "    ],\n"
#define S_JSON_LEND_FINAL "    ]\n"
#define S_JSON_TAIL "  ]\n}"

#define S_HTML_HEAD "<table style=\"border-collapse: collapse;\">\n"
#define S_HTML_LSTA "<tr>"
#define S_HTML_PCBR "<td style=\"color: rgb(%d, %d, %d); "\
                    "background: rgb(%d, %d, %d);\">&#%d;</td>"
#define S_HTML_PBLK "<td style=\"color: rgb(%d, %d, %d); "\
                    "background: rgb(%d, %d, %d);\">%s</td>"
#define S_HTML_PBBW "<td>%s</td>"
#define S_HTML_LEND "</tr>\n"
#define S_HTML_TAIL "</table>"

#define S_ANSI     "\033[%d;%dm"
#define S_ANSI_RGB "\033[38;2;%d;%d;%d;48;2;%d;%d;%dm"
#define S_ANSI_256 "\033[38;5;%d;48;5;%dm"
#define S_ANSI_RST "\033[0m"

#endif
