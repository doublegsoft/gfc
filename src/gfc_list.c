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

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include "gfc_list.h"
#include "gfc_type.h"
#include "gfc_gc.h"

#define INCREMENTAL_CAPACITY            10

/*!
** @private
*/

//typedef struct  gfc_list_item_s     gfc_list_item_t;
//typedef         gfc_list_item_t*    gfc_list_item_p;
//
//typedef struct gfc_list_item_s
//{
//  user_data                   data;
//  gfc_list_item_p             prev;
//  gfc_list_item_p             next;
//}
//gfc_list_item_t;

struct gfc_list_s
{
  size_t                      size;

  uintptr_t*                  pointers;
  size_t                      capacity;
};

/*!
** CREATE
*/
GFC_API gfc_list_p
gfc_list_new(void)
{
  gfc_list_p ret = (gfc_list_p) gfc_gc_malloc(sizeof(gfc_list_t), 1);

  ret->size = 0;
  ret->capacity = INCREMENTAL_CAPACITY;
  ret->pointers = (uintptr_t*) gfc_gc_malloc(sizeof(uintptr_t), ret->capacity);
  return ret;
}

/*!
** APPEND
*/
GFC_API void
gfc_list_append(gfc_list_p list, user_data data)
{
  list->pointers[list->size] = (uintptr_t) data;
  list->size++;

  if (list->size == list->capacity)
  {
    list->capacity += INCREMENTAL_CAPACITY;
    list->pointers = (uintptr_t*) gfc_gc_realloc(list->pointers, sizeof(uintptr_t), list->capacity);
  }
}

/*!
** SORT
*/
GFC_API void
gfc_list_sort(gfc_list_p list, gfc_list_compare compare)
{
  int i, j;
  for (i = 0; i < list->size - 1; i++)
    for (j = 0; j < list->size - i - 1; j++)
    {
      uintptr_t ptr_j = list->pointers[j];
      uintptr_t ptr_j_1 = list->pointers[j + 1];
//      gfc_list_item_t* item_j = (gfc_list_item_t*) ptr_j;
//      gfc_list_item_t* item_j_1 = (gfc_list_item_t*) ptr_j_1;
//      if (compare(item_j->data, item_j_1->data) == 1)
//      {
////        uintptr_t tmp = ptr_j;
////        ptr_j = ptr_j_1;
////        ptr_j_1 = tmp;
//        list->pointers[j] = (uintptr_t) item_j_1;
//        list->pointers[j + 1] = (uintptr_t) item_j;
//      }
    }
}

/*!
** @deprecated
*/
GFC_API void
gfc_list_remove(gfc_list_p list, uint index)
{
  if (index >= list->size)
    return;

  int i = 0;
  for (i = index; i < list->size - 1; i++)
  {
    user_data data = (user_data) list->pointers[i + 1];
    list->pointers[i] = (uintptr_t) data;
  }

  list->pointers[list->size - 1] = 0;

  list->size--;
}

/*!
** GET
*/
GFC_API user_data
gfc_list_get(gfc_list_p list, uint index)
{
  if (index >= list->size)
    return NULL;

  uintptr_t ptr = list->pointers[index];

  return (user_data) ptr;
}

/*!
** SIZE
*/
GFC_API uint
gfc_list_size(gfc_list_p list)
{
  return list->size;
}

/*!
** CLEAR
**
** @deprecated
*/
GFC_API void
gfc_list_clear(gfc_list_p list)
{
  int i;
  for (i = 0; i < list->size; i++)
    gfc_list_remove(list, 0);
  list->size = 0;
}

/*!
** DESTROY
*/
GFC_API void
gfc_list_free(gfc_list_p list)
{
//  gfc_list_clear(list);
  int rc = gfc_gc_free(list->pointers);
  assert(GFC_GC_OK == rc);
  rc = gfc_gc_free(list);
  assert(GFC_GC_OK == rc);
  list = NULL;
}

GFC_API void
gfc_list_deep_free(gfc_list_p list)
{
  int i;
  int size = gfc_list_size(list);
  for (i = 0; i < size; i++)
  {
    void* ptr = (void*)gfc_list_get(list, i);
    gfc_gc_free(ptr);
  }
  gfc_list_free(list);
}

