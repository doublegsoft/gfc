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

#ifndef __GFC_UUID_H__
#define __GFC_UUID_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "gfc_type.h"

#define UUID_STR_LENGTH (36)
#define UUID_T_LENGTH (16)

typedef unsigned char gfc_uuid_t[UUID_T_LENGTH];

/*!
** Clears uuid data.
**
** @param uuid
**        the uuid data
*/
GFC_API void
gfc_uuid_clear(gfc_uuid_t uuid);

/*!
** Compares two uuid data.
**
** @param uuid1
**        the first uuid data
**
** @param uuid2
**        the second uuid data
**
** @return 0 means equal
*/
GFC_API int
gfc_uuid_compare(const gfc_uuid_t uuid1, const gfc_uuid_t uuid2);


/*!
** Copies the source uuid to destination uuid
**
** @param dst
**        the destination uuid
**
** @param src
**        the source uuid
*/
GFC_API void
gfc_uuid_copy(gfc_uuid_t dst, const gfc_uuid_t src);

/*!
** Generates a system uuid.
**
** @param out
**        the uuid holder
*/
GFC_API void
gfc_uuid_generate(gfc_uuid_t out);

/*!
** Generates a random uuid.
**
** @param out
**        the uuid holder
*/
GFC_API void
gfc_uuid_random(gfc_uuid_t out);

/*!
** Checks the uuid data is null
**
** @param uuid
**        the uuid data
**
** @return 0 means null, 1 means not null
*/
GFC_API int
gfc_uuid_null(const gfc_uuid_t uuid);

/*!
** Parses a string to an uuid data.
**
** @param in
**        the uuid as a string
**
** @param uuid
**        the uuid data holder
*/
GFC_API int
gfc_uuid_parse(const char* in, gfc_uuid_t uuid);

/*!
** Converts uuid data to lowercase string.
**
** @param uuid
**        the uuid data
**
** @param out
**        the string holder
*/
GFC_API void
gfc_uuid_lower(const gfc_uuid_t uuid, char* out);

/*!
** Converts uuid data to uppercase string.
**
** @param uuid
**        the uuid data
**
** @param out
**        the string holder
*/
GFC_API void
gfc_uuid_upper(const gfc_uuid_t uuid, char* out);

#ifdef __cplusplus
}
#endif

#endif // __GFC_UUID_H__
