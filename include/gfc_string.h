/*
 * gfc
 *
 * Copyright (C) 2019 doublegsoft.open
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GFC_STRING_H__
#define __GFC_STRING_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gfc_type.h"
#include "gfc_list.h"

#ifdef _WIN32
#  ifdef _MSC_VER
#include <windows.h>
#  else
#include <iconv.h>
#  endif
#endif

#define GFC_STRING_INIT_SIZE  256

typedef struct gfc_string_s   gfc_string_t;

typedef gfc_string_t*         gfc_string_p;

struct gfc_string_s
{
  char*                   buffer;
};

/*!
** @brief
** Creates a new string object.
**
** @param str
**        the initializing string, maybe {@code NULL}
**
** @param len
**        the length of string as you know
**
** @return a new string object
*/
GFC_API gfc_string_p
gfc_string_new(const char* str);

/*!
** @brief
** Concatenates a new value to the given string object.
**
** @param str
**        the existing string object
**
** @param val
**        the string value to concatenate
**
** @param len
**        the string length as you know
*/
GFC_API void
gfc_string_concat(gfc_string_p str, const char* val);

/*!
** @brief
** Gets the length of string object.
**
** @param str
**        the existing string object
**
** @return the length of string object
*/
GFC_API size_t
gfc_string_length(gfc_string_p str);

/*!
** @brief
** Splits the string into strings by separator.
*/
GFC_API gfc_list_p
gfc_string_split(char* str, const char* sep);

/*!
** @brief
** Destroys a string object.
*/
GFC_API void
gfc_string_free(gfc_string_p str);

GFC_API void
gfc_string_print(gfc_string_p str);

/*！
** @brief
** Converts the source string to utf8 encoding string in destination string, especially in msvc
** environment.
** 
** @param src
**        the source string
** 
** @param dst
**        the utf8 encoding destination string
**
** @param len
**        the buffer length of destination string
*/
GFC_API void
gfc_string_utf8(const char* src, char* dst, int len);

GFC_API void
gfc_string_gbk(const char* src, char* dst, int len);

#ifdef __cplusplus
}
#endif

#endif
