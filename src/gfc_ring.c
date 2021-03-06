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

#include <stdlib.h>

#include "gfc_type.h"
#include "gfc_ring.h"

#define GFC_RING_INVALID_INDEX          -1
/*!
** @private
*/

typedef struct  gfc_ring_item_s     gfc_ring_item_t;
typedef         gfc_ring_item_t*    gfc_ring_item_p;

typedef struct gfc_ring_item_s
{
  user_data                   data;
  gfc_ring_item_p             prev;
  gfc_ring_item_p             next;
}
gfc_ring_item_t;

struct gfc_ring_s
{
  gfc_ring_item_p             head;

  /*!
  ** the setter pointer to item.
  */
  uint                        setter;

  /*!
  ** the getter pointer to item.
  */
  uint                        getter;

  uint                        capacity;
};

gfc_ring_p
gfc_ring_new(uint count)
{
  gfc_ring_p ret = (gfc_ring_p) malloc(sizeof(gfc_ring_t));
  ret->head = (gfc_ring_item_p) malloc(sizeof(gfc_ring_item_p) * count);
  ret->setter = 0;
  ret->getter = -1;
  ret->capacity = count;
  return ret;
}

void
gfc_ring_push(gfc_ring_p ring, user_data data)
{
  gfc_ring_item_p curr = &ring->head[ring->setter];
  curr->data = data;
  ring->setter++;
  if (ring->setter == ring->capacity)
    ring->setter = 0;
  if (ring->getter == GFC_RING_INVALID_INDEX)
    ring->getter = 0;
}

user_data
gfc_ring_pop(gfc_ring_p ring)
{
  if (ring->getter == GFC_RING_INVALID_INDEX)
    return NULL;
  user_data ret = ring->head[ring->getter].data;
  ring->getter++;
  if (ring->getter == ring->setter)
    ring->getter = GFC_RING_INVALID_INDEX;
  return ret;
}

void
gfc_ring_clear(gfc_ring_p ring)
{
  int i;
  for (i = 0; i < ring->capacity; i++)
  {
    gfc_ring_item_p item = &ring->head[i];
    if (item->data != NULL)
      free(item);
    ring->setter = 0;
    ring->getter = GFC_RING_INVALID_INDEX;
  }
}

void
gfc_ring_free(gfc_ring_p ring)
{
  gfc_ring_clear(ring);
  free(ring);
  ring = NULL;
}
