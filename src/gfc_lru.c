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
#include <assert.h>

#include "gfc_lru.h"
#include "gfc_type.h"
#include "gfc_gc.h"


// ------------------------------------------
// private functions
// ------------------------------------------
// MurmurHash2, by Austin Appleby
// http://sites.google.com/site/murmurhash/
static uint32_t
gfc_lru_hash(gfc_lru_p lru, void* key, uint32_t key_length) {
  uint32_t m = 0x5bd1e995;
  uint32_t r = 24;
  uint32_t h = lru->seed ^ key_length;
  char* data = (char *)key;

  while(key_length >= 4)
  {
    uint32_t k = *(uint32_t *)data;
    k *= m;
    k ^= k >> r;
    k *= m;
    h *= m;
    h ^= k;
    data += 4;
    key_length -= 4;
  }

  switch(key_length)
  {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
            h *= m;
  };

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  return h % lru->hash_table_size;
}

// compare a key against an existing item's key
static int
gfc_lru_compare(gfc_lru_item_p item, void* key, uint32_t key_length) {
  if(key_length != item->key_length)
    return 1;
  else
    return memcmp(key, item->key, key_length);
}

// remove an item and push it to the free items queue
static void
gfc_lru_item_remove(gfc_lru_p lru, gfc_lru_item_p prev, gfc_lru_item_p item, uint32_t hash_index) {
  if(prev)
    prev->next = item->next;
  else
    lru->items[hash_index] = (gfc_lru_item_p) item->next;

  // free memory and update the free memory counter
  lru->free_memory += item->value_length;
//  if (item->key)
//    free(item->key);
//  if (item->value)
//    free(item->value);
//  int rc = gfc_gc_free(item);
//  assert(GFC_GC_OK == rc);

  // push the item to the free items queue
  memset(item, 0, sizeof(gfc_lru_item_t));
  item->next = lru->free_items;
  lru->free_items = item;
}

// remove the least recently used item
// TODO: we can optimise this by finding the n lru items, where n = required_space / average_length
static void
gfc_lru_item_abandon(gfc_lru_p lru) {
  gfc_lru_item_p min_item = NULL, min_prev = NULL;
  gfc_lru_item_p item = NULL, prev = NULL;
  uint32_t i = 0, min_index = -1;
  uint64_t min_access_count = -1;

  for(; i < lru->hash_table_size; i++)
  {
    item = lru->items[i];
    prev = NULL;

    while(item) {
      if(item->access_count < min_access_count || min_access_count == -1) {
        min_access_count = item->access_count;
        min_item  = item;
        min_prev  = prev;
        min_index = i;
      }
      prev = item;
      item = item->next;
    }
  }

  if(min_item)
    gfc_lru_item_remove(lru, min_prev, min_item, min_index);
}

// pop an existing item off the free queue, or create a new one
static gfc_lru_item_p
gfc_lru_item_pop(gfc_lru_p lru) {
  gfc_lru_item_p item = NULL;

  if(lru->free_items)
  {
    item = lru->free_items;
    lru->free_items = item->next;
//    item = (gfc_lru_item_p) gfc_gc_malloc(sizeof(gfc_lru_item_t), 1);
  }
  else
    item = (gfc_lru_item_p) gfc_gc_malloc(sizeof(gfc_lru_item_t), 1);
  return item;
}

#define GFC_LRU_LOCK()    if(pthread_mutex_lock(lru->mutex)) {\
  perror("LRU Cache unable to obtain mutex lock");\
  return GFC_LRU_PTHREAD_ERROR;\
}

#define GFC_LRU_UNLOCK()  if(pthread_mutex_unlock(lru->mutex)) {\
  perror("LRU Cache unable to release mutex lock");\
  return GFC_LRU_PTHREAD_ERROR;\
}


// ------------------------------------------
// public api
// ------------------------------------------
GFC_API gfc_lru_p
gfc_lru_new(uint64_t cache_size, uint32_t average_length) {
  // create the cache
  gfc_lru_p lru = (gfc_lru_p) gfc_gc_malloc(sizeof(gfc_lru_t), 1);
  int rc;
  if(!lru) {
    perror("LRU Cache unable to create cache object");
    return NULL;
  }
  lru->hash_table_size      = cache_size / average_length;
  lru->average_item_length  = average_length;
  lru->free_memory          = cache_size;
  lru->total_memory         = cache_size;
  lru->seed                 = time(NULL);
  lru->free_items           = NULL;

  // size the hash table to a guestimate of the number of slots required (assuming a perfect hash)
  lru->items = (gfc_lru_item_p*) gfc_gc_malloc(sizeof(gfc_lru_item_p), lru->hash_table_size);
  if(!lru->items)
  {
    perror("LRU Cache unable to create cache hash table");
    rc = gfc_gc_free(lru);
    assert(GFC_GC_OK == rc);
    return NULL;
  }

  // all cache calls are guarded by a mutex
  lru->mutex = (pthread_mutex_t *) gfc_gc_malloc(sizeof(pthread_mutex_t), 1);
  if(pthread_mutex_init(lru->mutex, NULL)) {
    perror("LRU Cache unable to initialise mutex");
    int rc = gfc_gc_free(lru->items);
    assert(GFC_GC_OK == rc);
    rc = gfc_gc_free(lru);
    assert(GFC_GC_OK == rc);
    return NULL;
  }
  return lru;
}


GFC_API gfc_lru_error
gfc_lru_free(gfc_lru_p lru) {
  // free each of the cached items, and the hash table
  gfc_lru_item_p item = NULL, next = NULL;
  int rc;
  uint32_t i = 0;
  if(lru->items) {
    for(; i < lru->hash_table_size; i++) {
      item = lru->items[i];
      while(item)
      {
        next = (gfc_lru_item_p) item->next;
        rc = gfc_gc_free(item);
        assert(GFC_GC_OK == rc);
        item = next;
      }
    }
    rc = gfc_gc_free(lru->items);
    assert(GFC_GC_OK == rc);
  }

  // free the cache
  if(lru->mutex)
  {
    if(pthread_mutex_destroy(lru->mutex))
    {
      perror("LRU Cache unable to destroy mutex");
      return GFC_LRU_PTHREAD_ERROR;
    }
  }
  rc = gfc_gc_free(lru);
  assert(GFC_GC_OK == rc);

  return GFC_LRU_NO_ERROR;
}


GFC_API gfc_lru_error
gfc_lru_set(gfc_lru_p lru, void *key, uint32_t key_length, void *value, uint32_t value_length) {
  GFC_LRU_LOCK();

  // see if the key already exists
  uint32_t hash_index = gfc_lru_hash(lru, key, key_length), required = 0;
  gfc_lru_item_p item = NULL, prev = NULL;
  item = lru->items[hash_index];

  while(item && gfc_lru_compare(item, key, key_length))
  {
    prev = item;
    item = (gfc_lru_item_p) item->next;
  }


  if(item)
  {
    // update the value and value_lengths
    required = value_length - item->value_length;
//    free(item->value);
//    free(item->key);
    item->value = value;
    item->value_length = value_length;
  }
  else
  {
    // insert a new item
    item = gfc_lru_item_pop(lru);
    item->value = value;
    item->key = key;
    item->value_length = value_length;
    item->key_length = key_length;
    required = value_length;

    if(prev)
      prev->next = item;
    else
      lru->items[hash_index] = item;
  }
  item->access_count = ++lru->access_count;

  // remove as many items as necessary to free enough space
  if(required > 0 && required > lru->free_memory) {
    while(lru->free_memory < required)
      gfc_lru_item_abandon(lru);
  }
  lru->free_memory -= required;
  GFC_LRU_UNLOCK();
  return GFC_LRU_NO_ERROR;
}


GFC_API gfc_lru_error
gfc_lru_get(gfc_lru_p lru, void *key, uint32_t key_length, void** value) {
  GFC_LRU_LOCK();

  // loop until we find the item, or hit the end of a chain
  uint32_t hash_index = gfc_lru_hash(lru, key, key_length);
  gfc_lru_item_p item = lru->items[hash_index];

  while(item && gfc_lru_compare(item, key, key_length))
    item = (gfc_lru_item_p) item->next;

  if(item) {
    *value = item->value;
    item->access_count = ++lru->access_count;
  } else {
    *value = NULL;
  }

  GFC_LRU_UNLOCK();
  return GFC_LRU_NO_ERROR;
}


GFC_API gfc_lru_error
gfc_lru_remove(gfc_lru_p lru, void* key, uint32_t key_length) {
  GFC_LRU_LOCK();

  // loop until we find the item, or hit the end of a chain
  gfc_lru_item_p item = NULL, prev = NULL;
  uint32_t hash_index = gfc_lru_hash(lru, key, key_length);
  item = lru->items[hash_index];

  while(item && gfc_lru_compare(item, key, key_length))
  {
    prev = item;
    item = (gfc_lru_item_p) item->next;
  }

  if(item)
    gfc_lru_item_remove(lru, prev, item, hash_index);

  GFC_LRU_UNLOCK();
  return GFC_LRU_NO_ERROR;
}
