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

#ifndef __GFC_MT_H__
#define __GFC_MT_H__

#include "gfc_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct  gfc_mt_s     gfc_mt_t;
typedef         gfc_mt_t*    gfc_mt_p;

/*!
** Creates a threads context instance.
**
** @param count
**        the thread count
**
** @return a threads context instance
*/
GFC_API gfc_mt_p
gfc_mt_new(uint count);

/*!
** Sets the thread working function in threads context.
**
** @param threads
**        the threads handle
**
** @param fun
**        the thread working function
**
** @param data
**        the thread working function parameters
*/
GFC_API void
gfc_mt_do(gfc_mt_p threads, void*(*fun)(void* data), void* data);

/*!
** Frees a threads context instance.
**
** @param threads
**        threads context instance
*/
GFC_API void
gfc_mt_free(gfc_mt_p threads);

GFC_API int
gfc_mt_size(gfc_mt_p mt);

#ifdef __cplusplus
}
#endif

#endif // __GFC_MT_H__
