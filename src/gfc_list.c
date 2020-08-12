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
#include "gfc_list.h"

/*!
** @private
*/

typedef struct  gfc_list_item_s     gfc_list_item_t;
typedef         gfc_list_item_t*    gfc_list_item_p;

typedef struct gfc_list_item_s
{
  user_data                   data;
  gfc_list_item_p             prev;
  gfc_list_item_p             next;
}
gfc_list_item_t;

struct gfc_list_s
{
  gfc_list_item_p             head;
  gfc_list_item_p             tail;
  size_t                      size;
};

gfc_list_p
gfc_list_new(void)
{
  gfc_list_p ret = (gfc_list_p) malloc(sizeof(gfc_list_t));
  ret->head = ret->tail = NULL;
  ret->size = 0;
  return ret;
}

void
gfc_list_append(gfc_list_p list, user_data data)
{
  gfc_list_item_p item = (gfc_list_item_p) malloc(sizeof(gfc_list_item_t));

  item->data = data;
  if (list->head == NULL)
    list->head = item;

  if (list->tail == NULL)
    list->tail = item;
  else
  {
    item->prev = list->tail;
    list->tail->next = item;
    // shift tail
    list->tail = item;
    list->tail->next = NULL;
  }
  list->size++;
}

void
gfc_list_remove(gfc_list_p list, uint index)
{
  if (index >= list->size)
    return;

  int idx = 0;
  gfc_list_item_t *remove = list->head;
  while (idx != index)
  {
    remove = remove->next;
    idx++;
  }

  if (remove == list->head)
  {
    list->head = list->head->next;
    if (list->head != NULL)
      list->head->prev = NULL;
  }
  else if (remove == list->tail)
  {
    list->tail = list->tail->prev;
    list->tail->next = NULL;
  }
  else
  {
    remove->prev->next = remove->next;
    remove->next->prev = remove->prev;
  }
  free(remove);
  list->size--;
}

user_data
gfc_list_get(gfc_list_p list, uint index)
{
  if (index >= list->size)
    return NULL;
  int idx = 0;
  gfc_list_item_t *found = list->head;
  while (idx < index)
  {
    idx++;
    found = found->next;
  }
  return found->data;
}

uint
gfc_list_size(gfc_list_p list)
{
  return list->size;
}

void
gfc_list_clear(gfc_list_p list)
{
  int i;
  for (i = 0; i < list->size; i++)
    gfc_list_remove(list, 0);
}

void
gfc_list_free(gfc_list_p list)
{
  gfc_list_clear(list);
  free(list);
  list = NULL;
}
