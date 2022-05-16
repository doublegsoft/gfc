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

#include <stdlib.h>

#include "gfc_utf8.h"


GFC_API uint
gfc_utf8_length(char* str)
{
  uint    ret = 0;
  int     len = strlen(str);
  int     i   = 0;
  for (i = 0; i < len; i++)
  {
    char ch = str[i];
    if (ch == '\0') break;
    if ((ch & 0b11110000) == 0b11110000)
      i += 3;
    else if ((ch & 0b11100000) == 0b11100000)
      i += 2;
    else if ((ch & 0b11000000) == 0b11000000)
      i += 1;
    ret++;
  }
  return ret;
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


