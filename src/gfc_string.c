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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#  ifdef _MSC_VER
#include <windows.h>
#  else
#include <iconv.h>
#  endif
#endif

#include "gfc_string.h"
#include "gfc_type.h"

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
gfc_string_new(const char* str, int len)
{
  gfc_string_p ret = (gfc_string_p) malloc(sizeof(gfc_string_p));

  if (str == NULL || len == 0)
    ret->length = 1;
  else
    ret->length = len + 1;

  ret->buffer = (char*) malloc(sizeof(char) * ret->length);
  memcpy(ret->buffer, str, len);
  ret->buffer[ret->length - 1] = '\0';
  return ret;
}

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
gfc_string_concat(gfc_string_p str, const char* val, int len)
{
  if (val == NULL) return;
  
  int old_len = str->length;
  str->length += len;
  
  str->buffer = (char*) realloc(str->buffer, sizeof(char) * str->length);
  
  int i = 0;
  for (; i < len; i++)
    memset(str->buffer + old_len + i - 1, val[i], 1);

  str->buffer[str->length - 1] = '\0';
}

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
gfc_string_length(gfc_string_p str)
{
  return str->length - 1;
}

/*!
** @brief
** Destroys a string object.
*/
GFC_API void
gfc_string_free(gfc_string_p str)
{
  free(str->buffer);
  free(str);
}

GFC_API void
gfc_string_print(gfc_string_p str)
{
  printf("%s\n", str->buffer);
}

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
gfc_string_utf8(const char* src, char* dst, int len)
{
  if (src == NULL)
    return;
#ifdef _WIN32
  int ret = 0;
  wchar_t* wstr = NULL;
#  ifdef _MSC_VER
  int wlen = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, 0);
  if (wlen <= 0) return;

  wstr = (WCHAR*)malloc(wlen * 2);
  MultiByteToWideChar(CP_ACP, 0, src, -1, wstr, wlen);
  wlen = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
  if (len >= wlen)
  {
    ret = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, dst, wlen, NULL, NULL);
    dst[wlen] = 0;
  }
  if (ret <= 0)
  {
    free(wstr);
    return;
  }
#  else
  iconv_t icd;
  char* inptr = (char*) src;
  char* outptr = dst;
  size_t len_src = strlen(src);
  size_t len_dst = len;
  icd = iconv_open("UTF-8", "GBK");
  iconv(icd, &inptr, &len_src, &outptr, &len_dst);
  iconv_close(icd);
  dst[len_dst + 1] = '\0';
# endif

#else
  strcpy(dst, src);
#endif
}

GFC_API void
gfc_string_gbk(const char* src, char* dst, int len)
{
#ifdef _WIN32
#  ifdef _MSC_VER
  int n = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);

  wchar_t* utf8 = (wchar_t*)malloc(sizeof(wchar_t) * n);
  MultiByteToWideChar(CP_UTF8, 0, src, -1, utf8, n);

  n = WideCharToMultiByte(CP_ACP, 0, utf8, -1, NULL, 0, NULL, NULL);

  WideCharToMultiByte(CP_ACP, 0, utf8, -1, dst, n, NULL, NULL);

  free(utf8);
#  else
  iconv_t icd;
  char* inptr = (char*) src;
  char* outptr = dst;
  size_t len_src = strlen(src);
  size_t len_dst = len;
  icd = iconv_open("GBK", "UTF-8");
  iconv(icd, &inptr, &len_src, &outptr, &len_dst);
  iconv_close(icd);
  dst[len_dst + 1] = '\0';
#  endif
#else
  strcpy(dst, src);
#endif
}
