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
