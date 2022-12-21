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
** Copyright (C) 2022 doublegsoft.open
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

#ifndef __GFC_SHA1_H__
#define __GFC_SHA1_H__

#include <stdint.h>

#ifndef _SHA_enum_
#define _SHA_enum_
enum
{
  shaSuccess = 0,
  shaNull,            /* Null pointer parameter */
  shaInputTooLong,    /* input data too long */
  shaStateError       /* called Input after Result */
};
#endif
#define SHA1HashSize 20

/*
 *  This structure will hold context information for the SHA-1
 *  hashing operation
 */
typedef struct SHA1Context
{
  uint32_t intermediate_hash[SHA1HashSize/4];
  uint32_t length_low;
  uint32_t length_high;
  int_least16_t message_block_index;
  uint8_t message_block[64];
  int computed;
  int corrupted;
}
SHA1Context;

/*
 *  Function Prototypes
 */

int gfc_sha1_reset(SHA1Context*);
int gfc_sha1_input(SHA1Context*, const uint8_t*, unsigned int);
int gfc_sha1_result(SHA1Context*, uint8_t message_digest[SHA1HashSize]);


#endif /* __GFC_SHA1_H__ */
