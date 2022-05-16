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

#ifndef __GFC_MAP_H__
#define __GFC_MAP_H__

#include <stdlib.h>

#include "gfc_type.h"

#define GFC_ERROR_MAP_OK                0
#define GFC_ERROR_MAP_FULL              3001
#define GFC_ERROR_MAP_MISSING           3002
#define GFC_ERROR_MAP_OUT_OF_MEMORY     3003

typedef struct  gfc_map_s          gfc_map_t;
typedef         gfc_map_t*         gfc_map_p;

typedef int (*resovle)(char*, void*);

/*!
** @brief
**
** Creates a map instance.
**
** @return a map instance
*/
GFC_API gfc_map_p
gfc_map_new(void);

/*!
** @brief
**
** Puts a user data to the map.
**
** @param map
**        a map instance
**
** @param data
**        a user data
*/
GFC_API int
gfc_map_put(gfc_map_p map, const char* key, user_data data);

/*!
** @brief
**
** Removes an user data with the given index.
**
** @param map
**        a map instance
**
** @param index
**        an indexed user data to remove, zero-based.
*/
GFC_API user_data
gfc_map_pop(gfc_map_p map, const char* key);

/*!
** @brief
**
** Gets an user data with the given index in the map.
**
** @param map
**        a map instance
**
** @param key
**        the entry key
**
** @return the found user data or null
*/
GFC_API int
gfc_map_get(gfc_map_p map, const char* key, user_data* data);

/*!
** @brief
**
** Gets the size of map.
**
** @param map
**        a map instance
**
** @return the size of map
*/
GFC_API uint
gfc_map_size(gfc_map_p map);

/*!
** @brief
**
** Clears the items in the map.
**
** @param map
**        a map instance
*/
GFC_API void
gfc_map_clear(gfc_map_p map);

/*!
** @brief
**
** Frees the map instance and its memory occupation.
**
** @param map
**        a map to free
*/
GFC_API void
gfc_map_free(gfc_map_p map);

#endif // __GFC_MAP_H__
