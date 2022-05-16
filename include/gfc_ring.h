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

#ifndef __GFC_RING_H__
#define __GFC_RING_H__

#include <stdlib.h>

#include "gfc_type.h"

typedef struct  gfc_ring_s          gfc_ring_t;
typedef         gfc_ring_t*         gfc_ring_p;

/*!
** @brief 创建一个列表对象。
**
** @return 创建的列表对象
*/
GFC_API gfc_ring_p
gfc_ring_new(uint count);

/*!
** @brief 添加用户数据到环。
**
** @param ring 列表对象
**
** @param data 用户数据
*/
GFC_API void
gfc_ring_push(gfc_ring_p ring, user_data data);

/*!
** @brief 获取环中的用户数据。
**
** @param ring 列表对象
*/
GFC_API user_data
gfc_ring_pop(gfc_ring_p ring);

/*!
** @brief 清空一个列表对象。
**
** @param ring 列表对象
*/
GFC_API void
gfc_ring_clear(gfc_ring_p ring);

/*!
** @brief 销毁一个列表对象。
**
** @param 销毁的列表对象
*/
GFC_API void
gfc_ring_free(gfc_ring_p ring);

#endif // __GFC_RING_H__
