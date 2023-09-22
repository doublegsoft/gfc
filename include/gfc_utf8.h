/*
** gfc
**
** Copyright (C) 2019 doublegsoft.open
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __GFC_UTF8_H__
#define __GFC_UTF8_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include "gfc_type.h"

typedef struct  gfc_utf8_char_s       gfc_utf8_char_t;
typedef         gfc_utf8_char_t*      gfc_utf8_char_p;

typedef struct  gfc_utf8_string_s     gfc_utf8_string_t;
typedef         gfc_utf8_string_t*    gfc_utf8_string_p;

struct gfc_utf8_char_s
{
  byte bytes[4];

  int length;
};

struct gfc_utf8_string_s
{
  gfc_utf8_char_p chars;

  int length;
};

GFC_API uint
gfc_utf8_length(const char* str);

GFC_API uint
gfc_utf8_initial(const char* str, char* buff);

GFC_API uint
gfc_utf8_convert(char* str, gfc_utf8_string_p utf8_str);

#ifdef __cplusplus
}
#endif

#endif // __GFC_UTF8_H__
