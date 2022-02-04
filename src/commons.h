/*
 * yaitaa - yet another image to ascii art converter
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
#ifndef _COMMONS_H_
#define _COMMONS_H_

#include "args.h"

#define CLAMP(min, val, max) ((val)>(max)?(max):((val)<(min)?(min):(val)))

void c_fatal(int code, const char *reason);
void m_prepare_dither(asc_state_t *state);
palette_t *get_palette_by_id(asc_style_t stl);

#endif
