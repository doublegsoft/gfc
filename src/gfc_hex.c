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

#include <string.h>

#include "gfc_hex.h"

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
gfc_hex_to_int(char ch)
{
  switch (ch)
  {
  case '0':
    return 0;
  case '1':
    return 1;
  case '2':
    return 2;
  case '3':
    return 3;
  case '4':
    return 4;
  case '5':
    return 5;
  case '6':
    return 6;
  case '7':
    return 7;
  case '8':
    return 8;
  case '9':
    return 9;
  case 'A':
    return 10;
  case 'B':
    return 11;
  case 'C':
    return 12;
  case 'D':
    return 13;
  case 'E':
    return 14;
  case 'F':
    return 15;
  default:
    return 16;
  }
}

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
gfc_hex_to_char(int dec)
{
  switch (dec)
  {
  case 0:
    return '0';
  case 1:
    return '1';
  case 2:
    return '2';
  case 3:
    return '3';
  case 4:
    return '4';
  case 5:
    return '5';
  case 6:
    return '6';
  case 7:
    return '7';
  case 8:
    return '8';
  case 9:
    return '9';
  case 10:
    return 'A';
  case 11:
    return 'B';
  case 12:
    return 'C';
  case 13:
    return 'D';
  case 14:
    return 'E';
  case 15:
    return 'F';
  default:
    return 0;
  }
}

GFC_API void
gfc_hex_to_bits(char hex, char* bits)
{
  switch (hex)
  {
  case '0':
    strcpy(bits, "0000");
    return;
  case '1':
    strcpy(bits, "0001");
    return;
  case '2':
    strcpy(bits, "0010");
    return;
  case '3':
    strcpy(bits, "0011");
    return;
  case '4':
    strcpy(bits, "0100");
    return;
  case '5':
    strcpy(bits, "0101");
    return;
  case '6':
    strcpy(bits, "0110");
    return;
  case '7':
    strcpy(bits, "0111");
    return;
  case '8':
    strcpy(bits, "1000");
    return;
  case '9':
    strcpy(bits, "1001");
    return;
  case 'A':
    strcpy(bits, "1010");
    return;
  case 'B':
    strcpy(bits, "1011");
    return;
  case 'C':
    strcpy(bits, "1100");
    return;
  case 'D':
    strcpy(bits, "1101");
    return;
  case 'E':
    strcpy(bits, "1110");
    return;
  case 'F':
    strcpy(bits, "1111");
    return;
  default:
    assert(1 == 0);
    return;
  }
}

GFC_API void
gfc_hex_to_string(byte* bytes, int length, char* hexstring)
{
  int i = 0;
  int j = 0;

  for (; i < length; i++, j += 2)
  {
    char first = gfc_hex_to_char((bytes[i] & 0xF0) >> 4);
    char second = gfc_hex_to_char(bytes[i] & 0x0F);
    hexstring[j] = first;
    hexstring[j + 1] = second;
  }
  hexstring[j] = '\0';

}

GFC_API void
gfc_hex_to_bytes(const char* hexstring, byte* bytes, int* length)
{
  int i = 0;
  int len = strlen(hexstring);
  *length = len / 2;
  for (; i < *length; i++)
    bytes[i] = gfc_hex_to_int(hexstring[i * 2]) * 16 + gfc_hex_to_int(hexstring[i * 2 + 1]);
}

GFC_API int
gfc_hex_from_bytes_to_int(const char* hexstring, int start, int length, int radix)
{
  int i = 0;
  int result = 0;

  const char* decimal_string = hexstring + start;
  for (; i < length; i++)
  {
    char ch = decimal_string[i];
    int val = gfc_hex_to_int(ch);
    result += pow(radix, (double) length - i - 1) * val;
  }
  return result;
}

GFC_API char
gfc_hex_from_bits_to_char(char* bits)
{
  if (strcmp(bits, "0000") == 0)
    return '0';
  else if (strcmp(bits, "0001") == 0)
    return '1';
  else if (strcmp(bits, "0010") == 0)
    return '2';
  else if (strcmp(bits, "0011") == 0)
    return '3';
  else if (strcmp(bits, "0100") == 0)
    return '4';
  else if (strcmp(bits, "0101") == 0)
    return '5';
  else if (strcmp(bits, "0110") == 0)
    return '6';
  else if (strcmp(bits, "0111") == 0)
    return '7';
  else if (strcmp(bits, "1000") == 0)
    return '8';
  else if (strcmp(bits, "1001") == 0)
    return '9';
  else if (strcmp(bits, "1010") == 0)
    return 'A';
  else if (strcmp(bits, "1011") == 0)
    return 'B';
  else if (strcmp(bits, "1100") == 0)
    return 'C';
  else if (strcmp(bits, "1101") == 0)
    return 'D';
  else if (strcmp(bits, "1110") == 0)
    return 'E';
  else if (strcmp(bits, "1111") == 0)
    return 'F';
  else {
    assert(1 == 0);
    return ' ';
  }
}

GFC_API void
gfc_hex_substring(const char* source, uint start, uint length, char* dest)
{
  int i = 0;
  char* str = (char*) source;
  str += start;
  for (; i < length; i++)
    dest[i] = str[i];
  dest[i] = '\0';
}

GFC_API void
gfc_hex_parity(byte* bytes, uint bytes_length)
{
  uint i = 0;
  for (i = 0; i < bytes_length; i++)
  {
    byte b = bytes[i];
    b = b ^ (b >> 4 | b << 4);
    b = b ^ (b >> 2);
    b = b ^ (b >> 1);
    b = b & 1;
    if (!(b & 1))
    {
      if ((bytes[i] & 0x0F) == 0x0F)
        bytes[i] -= 1;
      else
        bytes[i] += 1;
    }
  }
}

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
gfc_asc_to_bcd(const char* str, uint* bcd_len)
{
  int str_len = strlen(str);
  int residue = str_len % 2;
  if (residue == 0)
    *bcd_len = str_len / 2;
  else
    *bcd_len = str_len / 2 + 1;

  int high, low;
  byte* ret = (byte *) malloc(*bcd_len);
  memset(ret, 0, *bcd_len);
  for (size_t i = 0; i < *bcd_len; i++)
  {
    high = i * 2;
    low = i * 2 + 1;
    // 高字节
    // 为什么执行两遍才成功
    ret[i] = (str[high] - '0') << 4;
    ret[i] = (str[high] - '0') << 4;
    if (low < str_len)
    {
      // 低字节，避免溢出
      ret[i] |= ((str[low] - '0'));
    }
  }
  return ret;
}

/*!
** Converts decimal value to bcd-encoded decimal value.
**
** @param size
**        the decimal value
**
** @return the bcd decimal value
*/
GFC_API int
gfc_dec_to_bcd(int dec)
{
  uint times = dec / 10;
  uint residue = dec % 10;
  return times * 16 + residue;
}
