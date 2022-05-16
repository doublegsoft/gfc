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

#ifndef __GFC_HEX_H__
#define __GFC_HEX_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "gfc_type.h"

/*!
** @brief
**
** Converts hex character to integer.
**
** @param ch
**        the character
**
** @return the integer
*/
GFC_API int
gfc_hex_to_int(char ch);

/*!
** @brief
**
** Converts integer to hex character, and integer value should be between 0 and 15.
**
** @param dec
**        the integer
**
** @return the hex character, '0'..'F'
*/
GFC_API char
gfc_hex_to_char(int dec);

GFC_API void
gfc_hex_to_bits(char hex, char* bits);

GFC_API void
gfc_hex_to_string(byte* bytes, int length, char* hexstring);

GFC_API void
gfc_hex_to_bytes(const char* hexstring, byte* bytes, int* length);

GFC_API int
gfc_hex_from_bytes_to_int(const char* hexstring, int start, int length, int radix);

GFC_API char
gfc_hex_from_bits_to_char(char* bits);

GFC_API void
gfc_hex_substring(const char* source, uint start, uint length, char* dest);

GFC_API void
gfc_hex_parity(byte* bytes, uint bytes_length);

/*!
** Converts the asc string to bcd-encoded byte array.
**
** @param str
**        the asc string
**
** @param bcd_len
**        the returned bcd-encoded byte length
**
** @return the bcd-encoded byte array
*/
GFC_API byte*
gfc_asc_to_bcd(const char* str, uint* bcd_len);

/*!
** Converts decimal value to bcd-encoded decimal value.
**
** @param size
**        the decimal value
**
** @return the bcd decimal value
*/
GFC_API int
gfc_dec_to_bcd(int dec);


#ifdef __cplusplus
}
#endif

#endif // __GFC_HEX_H__
