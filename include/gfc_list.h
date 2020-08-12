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

#ifndef __GFC_LIST_H__
#define __GFC_LIST_H__

#include <stdlib.h>

#include "gfc_type.h"

typedef struct  gfc_list_s          gfc_list_t;
typedef         gfc_list_t*         gfc_list_p;

/*!
** @brief
**
** Creates a list instance.
**
** @return a list instance
*/
gfc_list_p
gfc_list_new(void);

/*!
** @brief
**
** Appends a user data to the list.
**
** @param list
**        a list instance
**
** @param data
**        a user data
*/
void
gfc_list_append(gfc_list_p list, user_data data);

/*!
** @brief
**
** Removes an user data with the given index.
**
** @param list
**        a list instance
**
** @param index
**        an indexed user data to remove, zero-based.
*/
void
gfc_list_remove(gfc_list_p list, uint index);

/*!
** @brief
**
** Gets an user data with the given index in the list.
**
** @param list
**        a list instance
**
** @param index
**        the user data index
**
** @return the found user data or null
*/
user_data
gfc_list_get(gfc_list_p list, uint index);

/*!
** @brief
**
** Gets the size of list.
**
** @param list
**        a list instance
**
** @return the size of list
*/
uint
gfc_list_size(gfc_list_p list);

/*!
** @brief
**
** Clears the items in the list.
**
** @param list
**        a list instance
*/
void
gfc_list_clear(gfc_list_p list);

/*!
** @brief
**
** Frees the list instance and its memory occupation.
**
** @param list
**        a list to free
*/
void
gfc_list_free(gfc_list_p list);

#endif // __GFC_LIST_H__
