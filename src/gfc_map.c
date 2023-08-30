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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "gfc_type.h"
#include "gfc_map.h"
#include "gfc_gc.h"


#define INITIAL_SIZE (256)
#define MAX_CHAIN_LENGTH (8)

typedef struct  gfc_map_element_s          gfc_map_element_t;
typedef         gfc_map_element_t*         gfc_map_element_p;

/* We need to keep keys and values */
struct gfc_map_element_s
{
  char key[1024];
  int in_use;
  user_data data;
};

/* A hashmap has some maximum size and current size,
 * as well as the data to hold. */
struct gfc_map_s
{
  int table_size;
  int size;
  gfc_map_element_p data;
};

/*
 * Return an empty hashmap, or NULL on failure.
 */
GFC_API gfc_map_p
gfc_map_new() {
  gfc_map_p m = (gfc_map_p) gfc_gc_malloc(sizeof(gfc_map_t), 1);
  if(!m) goto err;

  m->data = (gfc_map_element_p) gfc_gc_malloc(sizeof(gfc_map_element_t), INITIAL_SIZE);
  if(!m->data) goto err;

  // NOTE: INIT VERY IMPORTANT
  for (int i = 0; i < INITIAL_SIZE; i++)
    m->data[i] = NULL;

  m->table_size = INITIAL_SIZE;
  m->size = 0;

  return m;

err:
  if (m)
    gfc_map_free(m);
  return NULL;
}

static unsigned long crc32_tab[] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

/* Return a 32-bit CRC of the contents of the buffer. */

unsigned long crc32(const unsigned char *s, unsigned int len)
{
  unsigned int i;
  unsigned long crc32val;

  crc32val = 0;
  for (i = 0;  i < len;  i ++)
  {
    crc32val = crc32_tab[(crc32val ^ s[i]) & 0xff] ^ (crc32val >> 8);
  }
  return crc32val;
}

/*
 * Hashing function for a string
 */
unsigned int
gfc_map_hash_int(gfc_map_p map, char* keystring){

  unsigned long key = crc32((unsigned char*)(keystring), strlen(keystring));

  /* Robert Jenkins' 32 bit Mix Function */
  key += (key << 12);
  key ^= (key >> 22);
  key += (key << 4);
  key ^= (key >> 9);
  key += (key << 10);
  key ^= (key >> 2);
  key += (key << 7);
  key ^= (key >> 12);

  /* Knuth's Multiplicative Method */
  key = (key >> 3) * 2654435761;

  return key % map->table_size;
}

/*
 * Return the integer of the location in data
 * to store the point to the item, or MAP_FULL.
 */
int
gfc_map_hash(gfc_map_p map, char* key){
  int curr;
  int i;

  /* If full, return immediately */
  if(map->size >= (map->table_size/2)) return GFC_ERROR_MAP_FULL;

  /* Find the best index */
  curr = gfc_map_hash_int(map, key);

  /* Linear probing */
  for(i = 0; i< MAX_CHAIN_LENGTH; i++){
    if(map->data[curr].in_use == 0)
      return curr;

    if(map->data[curr].in_use == 1 && (strcmp(map->data[curr].key,key)==0))
      return curr;

    curr = (curr + 1) % map->table_size;
  }

  return GFC_ERROR_MAP_FULL;
}

/*
 * Doubles the size of the hashmap, and rehashes all the elements
 */
int gfc_map_rehash(gfc_map_p map){
  int i;
  int old_size;
  gfc_map_element_p curr;

  gfc_map_element_p temp = (gfc_map_element_p)
        calloc(2 * map->table_size, sizeof(gfc_map_element_t));
  if(!temp) return GFC_ERROR_MAP_OUT_OF_MEMORY;

  /* Update the array */
  curr = map->data;
  map->data = temp;

  /* Update the size */
  old_size = map->table_size;
  map->table_size = 2 * map->table_size;
  map->size = 0;

  /* Rehash the elements */
  for(i = 0; i < old_size; i++){
    int status;

    if (curr[i].in_use == 0)
      continue;

    status = gfc_map_put(map, curr[i].key, curr[i].data);
    if (status != GFC_ERROR_MAP_OK)
      return status;
  }

  free(curr);

  return GFC_ERROR_MAP_OK;
}

/*!
** Add a pointer to the hashmap with some key
*/
GFC_API int
gfc_map_put(gfc_map_p map, const char* key, user_data value)
{
  int index;

  /* Find a place to put our value */
  index = gfc_map_hash(map, (char*)key);
  while(index == GFC_ERROR_MAP_FULL){
    if (gfc_map_rehash(map) == GFC_ERROR_MAP_OUT_OF_MEMORY) {
      return GFC_ERROR_MAP_OUT_OF_MEMORY;
    }
    index = gfc_map_hash(map, (char*)key);
  }

  /* Set the data */
  map->data[index].data = value;
  strcpy(map->data[index].key, key);
  map->data[index].in_use = 1;
  map->size++;

  return GFC_ERROR_MAP_OK;
}

/*
 * Get your pointer out of the hashmap with a key
 */
GFC_API int
gfc_map_get(gfc_map_p map, const char* key, user_data* arg)
{
  int curr;
  int i;

  curr = gfc_map_hash_int(map, (char*)key);

  /* Linear probing, if necessary */
  for(i = 0; i < MAX_CHAIN_LENGTH; i++){
    int in_use = map->data[curr].in_use;
    if (in_use == 1)
    {
      if (strcmp(map->data[curr].key,key)==0)
      {
        *arg = (map->data[curr].data);
        return GFC_ERROR_MAP_OK;
      }
    }
    curr = (curr + 1) % map->table_size;
  }

  *arg = NULL;

  return GFC_ERROR_MAP_MISSING;
}

/*
 * Iterate the function parameter over each element in the hashmap.  The
 * additional any_t argument is passed to the function as its first
 * argument and the hashmap element is the second.
 */
int
gfc_map_iterate(gfc_map_p map, int (*resolve)(const char*, user_data*)) {
  int i;

  if (gfc_map_size(map) <= 0)
    return GFC_ERROR_MAP_MISSING;

  /* Linear probing */
  for(i = 0; i < map->table_size; i++)
  {
//    if(map->data[i].in_use != 0) {
    const char* key = map->data[i].key;
    user_data data = (user_data) (map->data[i].data);
    int status = resolve(key, &data);
    map->data[i].data = NULL;
    map->data[i].in_use = 0;
    if (status != GFC_ERROR_MAP_OK) {
      return status;
    }
  }

  return GFC_ERROR_MAP_OK;
}

/*
 * Remove an element with that key from the map
 */
int
gfc_map_remove(gfc_map_p map, char* key){
  int i;
  int curr;

  /* Find key */
  curr = gfc_map_hash_int(map, key);

  /* Linear probing, if necessary */
  for(i = 0; i < MAX_CHAIN_LENGTH; i++){
    int in_use = map->data[curr].in_use;
    if (in_use == 1){
      if (strcmp(map->data[curr].key,key)==0){
        /* Blank out the fields */
        map->data[curr].in_use = 0;
        map->data[curr].data = NULL;
        map->data[curr].key[0] = '\0';

        /* Reduce the size */
        map->size--;
        return GFC_ERROR_MAP_OK;
      }
    }
    curr = (curr + 1) % map->table_size;
  }

  /* Data not found */
  return GFC_ERROR_MAP_MISSING;
}

/*!
** Deallocates the map instance.
*/
GFC_API void
gfc_map_free(gfc_map_p map)
{

  int rc = gfc_gc_free(map->data);
  assert(GFC_GC_OK == rc);

  rc = gfc_gc_free(map);
  assert(GFC_GC_OK == rc);
  map = NULL;
}

/*!
** Gets the length of map.
*/
uint
gfc_map_size(gfc_map_p map)
{
  if(map != NULL) return map->size;
  else return 0;
}

static int
gfc_map_item_free(const char* key, user_data* data)
{
  if (*data != NULL) free(*data);
  *data = NULL;
  return 0;
}

void
gfc_map_deep_free(gfc_map_p map)
{
  gfc_map_iterate(map, gfc_map_item_free);
  gfc_map_free(map);
}
