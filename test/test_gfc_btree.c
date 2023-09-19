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
#include <stdio.h>
#include <stdlib.h>

#include "gfc_btree.h"
#include "gfc_gc.h"

static int
compare(const user_data a, const user_data b)
{
  int* i1 = (int*) a;
  int* i2 = (int*) b;

  if (*i1 < *i2) return -1;
  if (*i1 > *i2) return 1;
  return 0;
}

int
main(int argc, char* argv[])
{
  gfc_gc_init();

  gfc_btree_p btree = gfc_btree_new(compare);

  for (int i = 0; i < 100; i++)
  {
    int* ptr = (int*) malloc(sizeof(int));
    *ptr = (99 - i);
    gfc_btree_append(btree, ptr);
  }

  gfc_btree_transverse(btree);
}
