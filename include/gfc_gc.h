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
#ifndef __GFC_GC_H__
#define __GFC_GC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "gfc_type.h"

#define GFC_GC_OK                               0
#define GFC_GC_IS_NULL                          204
#define GFC_GC_NOT_FOUND                        404
#define GFC_GC_KERNEL_ERROR                     500

/*!
** initializes gc context.
*/
GFC_API void
gfc_gc_init(void);

/*!
** closes and free gc context.
*/
GFC_API void
gfc_gc_close(void);

/*!
** gets total byte size which is managed by gc context.
*/
GFC_API size_t
gfc_gc_total(void);

GFC_API size_t
gfc_gc_length(void);

/*!
** free memory which the ptr points to.
*/
GFC_API int
gfc_gc_free(void* ptr);

/*!
** allocates a block of memory.
*/
GFC_API void*
gfc_gc_malloc(size_t size, size_t len);

GFC_API char*
gfc_gc_dup(const char* str);

/*!
** reallocates a new size block of memory to the pointer.
*/
void*
gfc_gc_realloc(void* ptr, size_t size, size_t len);


// can fail to produce an aligned result if alignment does not divide 2 * size_t
int
gfc_gc_memalign(void **memptr, size_t alignment, size_t size);

void*
gfc_gc_calloc(size_t count, size_t size);

//void
//gfc_gc_free(void *p);


//void*
//gfc_gc_realloc(void *p, size_t sz);

size_t gfc_gc_used();

#ifdef __cplusplus
}
#endif

#endif // __GFC_GC_H__
