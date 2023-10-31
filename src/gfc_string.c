/*
**           .d888
**          d88P"
**          888
**  .d88b.  888888 .d8888b
** d88P"88b 888   d88P"
** 888  888 888   888
** Y88b 888 888   Y88b.
**  "Y88888 888    "Y8888P
**      888
** Y8b d88P
**  "Y88P"
**
** Copyright (C) 2023 doublegsoft.open
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef _WIN32
#  ifdef _MSC_VER
#include <windows.h>
#  else
#include <iconv.h>
#  endif
#endif

#include "gfc_string.h"
#include "gfc_type.h"
#include "gfc_gc.h"

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
gfc_string_new(const char* str)
{
  if (str == NULL) return NULL;

  gfc_string_p ret = (gfc_string_p) gfc_gc_malloc(sizeof(gfc_string_p), 1);
  int len = strlen(str);
  if (len == 0)
    len = 1;
  else
    len = len + 1;

  ret->buffer = (char*) gfc_gc_malloc(sizeof(char), len);
  memcpy(ret->buffer, str, len);
  ret->buffer[len - 1] = '\0';
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
gfc_string_concat(gfc_string_p str, const char* val)
{
  if (val == NULL) return;
  int len = strlen(val);
  int old_len = strlen(str->buffer);
  
  str->buffer = (char*) gfc_gc_realloc(str->buffer, sizeof(char), (old_len + len + 1));
  
  int i = 0;
  for (; i < len; i++)
    memset(str->buffer + old_len + i, val[i], 1);

  str->buffer[old_len + len] = '\0';
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
  return strlen(str->buffer);
}

/*!
** @brief
** Destroys a string object.
*/
GFC_API void
gfc_string_free(gfc_string_p str)
{
  int rc = gfc_gc_free(str->buffer);
  assert(GFC_GC_OK == rc);

  rc = gfc_gc_free(str);
  assert(GFC_GC_OK == rc);

  str = NULL;
}

GFC_API void
gfc_string_print(gfc_string_p str)
{
  printf("%s\n", str->buffer);
}

GFC_API gfc_list_p
gfc_string_split(char* str, const char* sep)
{
  gfc_list_p ret = gfc_list_new();
  char* token = strtok(str, sep);
  while(token != NULL)
  {
    gfc_list_append(ret, gfc_gc_dup(token));
    token = strtok(NULL, sep);
  }
  return ret;
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
