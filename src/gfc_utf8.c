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
#include <stdint.h>
#include <stdlib.h>

#include "gfc_utf8.h"

static int32_t
gfc_utf8_continuation(char c) {
  return (c & 0xc0) == 0x80;
}

static int32_t
gfc_utf8_single(const char * c) {
  return (c[0] & 0x80) == 0x0;
}

static int32_t
gfc_utf8_double(const char * c) {
  return (c[0] & 0xe0) == 0xc0 && gfc_utf8_continuation(c[1]);
}

static int32_t
gfc_utf8_triple(const char * c) {
  return (c[0] & 0xf0) == 0xe0 && gfc_utf8_continuation(c[1]) && gfc_utf8_continuation(c[2]);
}

static int32_t
gfc_utf8_quadruple(const char * c) {
  return (c[0] & 0xf8) == 0xf0 && gfc_utf8_continuation(c[1]) && gfc_utf8_continuation(c[2]) && gfc_utf8_continuation(c[3]);
}

GFC_API uint
gfc_utf8_length(const char* str)
{
  size_t i = 0, len = 0;
  while(str[i])
  {
    if (!gfc_utf8_continuation(str[i])) ++len;
    ++i;
  }
  return len;
}

GFC_API uint
gfc_utf8_initial(const char* str, char* buff)
{
  int     i       = 0;
  int     size    = 0;
  if (str == NULL)
    return 0;
  char ch = str[0];
  if (ch == '\0')
    return 0;
  if (gfc_utf8_single(str))
    size = 1;
  else if (gfc_utf8_double(str))
    size = 2;
  else if (gfc_utf8_triple(str))
    size = 3;
  else if (gfc_utf8_quadruple(str))
    size = 4;

  for (i = 0; i < size; i++)
    buff[i] = str[i];
  buff[i + 1] = '\0';
  return i;
}

GFC_API uint
gfc_utf8_convert(char* str, gfc_utf8_string_p str_utf8)
{
  int   len     = strlen(str);
  int   utf8len = gfc_utf8_length(str);
  int   i       = 0;
  int   index   = 0;

  str_utf8->chars   = (gfc_utf8_char_p) malloc(sizeof(gfc_utf8_char_t) * utf8len);
  str_utf8->length  = utf8len;

  for (i = 0; i < len; i++)
  {
    char ch = str[i];
    if (ch == '\0') break;
    if ((ch & 0b11110000) == 0b11110000)
    {
      str_utf8->chars[index].bytes[0] = ch;
      str_utf8->chars[index].bytes[1] = str[i + 1];
      str_utf8->chars[index].bytes[2] = str[i + 2];
      str_utf8->chars[index].bytes[3] = str[i + 3];
      str_utf8->length = 4;
      i += 3;
    }
    else if ((ch & 0b11100000) == 0b11100000)
    {
      str_utf8->chars[index].bytes[0] = ch;
      str_utf8->chars[index].bytes[1] = str[i + 1];
      str_utf8->chars[index].bytes[2] = str[i + 2];
      str_utf8->length = 3;
      i += 2;
    }
    else if ((ch & 0b11000000) == 0b11000000)
    {
      str_utf8->chars[index].bytes[0] = ch;
      str_utf8->chars[index].bytes[1] = str[i + 1];
      str_utf8->length = 2;
      i += 1;
    }
    else
    {
      str_utf8->chars[index].bytes[0] = ch;
      str_utf8->length = 1;
    }
    index++;
  }

  return utf8len;
}


