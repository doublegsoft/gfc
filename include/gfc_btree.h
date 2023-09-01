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
#ifndef BTREE_H
#define BTREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "gfc_type.h"

typedef struct gfc_btree_s*     gfc_btree_p;

// btree_new returns a new B-tree.
//
// Param elsize is the size of each element in the tree. Every element that
// is inserted, deleted, or searched will be this size.
// Param max_items is the maximum number of items per node. Setting this to
// zero will default to 256.
// Param compare is a function that compares items in the tree. See the 
// qsort stdlib function for an example of how this function works.
// Param udata is user-defined data that is passed to the compare callback
// and the item callbacks defined in btree_set_item_callbacks.
//
// The btree must be freed with btree_free(). 
gfc_btree_p
gfc_btree_new(int (*compare)(const user_data a, const user_data b));

void
gfc_btree_append(gfc_btree_p ctx, user_data data);

void
gfc_btree_transverse(gfc_btree_p ctx);

#endif
