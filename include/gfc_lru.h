/*
** gfc
**
** Copyright (C) 2019 doubelgsoft.open
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
** along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef __GFC_LRU_H__
#define __GFC_LRU_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// #include <err.h>

#include <pthread.h>
#include <stdint.h>
#include <time.h>

#include "gfc_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
  GFC_LRU_NO_ERROR = 0,
  GFC_LRU_MISSING_CACHE,
  GFC_LRU_MISSING_KEY,
  GFC_LRU_MISSING_VALUE,
  GFC_LRU_PTHREAD_ERROR,
  GFC_LRU_VALUE_TOO_LARGE
}
gfc_lru_error;


typedef struct  gfc_lru_item_s        gfc_lru_item_t;
typedef         gfc_lru_item_t*       gfc_lru_item_p;

typedef struct  gfc_lru_s             gfc_lru_t;
typedef         gfc_lru_t*            gfc_lru_p;

struct gfc_lru_item_s
{
  void*       value;
  void*       key;
  uint32_t    value_length;
  uint32_t    key_length;
  uint64_t    access_count;
  void*       next;
};

struct gfc_lru_s
{
  gfc_lru_item_p*   items;
  uint64_t          access_count;
  uint64_t          free_memory;
  uint64_t          total_memory;
  uint64_t          average_item_length;
  uint32_t          hash_table_size;
  time_t            seed;
  gfc_lru_item_p    free_items;
  pthread_mutex_t*  mutex;
};


GFC_API gfc_lru_p
gfc_lru_new(uint64_t cache_size, uint32_t average_length);

GFC_API gfc_lru_error
gfc_lru_free(gfc_lru_p lru);

GFC_API gfc_lru_error
gfc_lru_set(gfc_lru_p lru, void* key, uint32_t key_length, void *value, uint32_t value_length);

GFC_API gfc_lru_error
gfc_lru_get(gfc_lru_p lru, void* key, uint32_t key_length, void **value);

GFC_API gfc_lru_error
gfc_lru_remove(gfc_lru_p lru, void* key, uint32_t key_length);


#ifdef __cplusplus
}
#endif

#endif // __GFC_LRU_H__
