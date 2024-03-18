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
#include <assert.h>

#include "gfc.h"


int
main()
{
  gfc_gc_init();
  gfc_map_p map1 = gfc_map_new();
  gfc_map_p map2 = gfc_map_new();

  gfc_string_p str1 = gfc_string_new("hello, str1");
  gfc_string_p str2 = gfc_string_new("hello, str2");
  gfc_string_p str3 = gfc_string_new("hello, str3");
  gfc_string_p str4 = gfc_string_new("hello, str4");

  gfc_map_put(map1, "hello", strdup(str1->buffer));
  gfc_map_put(map1, "world", strdup(str2->buffer));

  printf("map1 size: %d\n", gfc_map_size(map1));

  gfc_map_put(map2, "hello", strdup(str3->buffer));
  gfc_map_put(map2, "world", strdup(str4->buffer));
  printf("map2 size: %d\n", gfc_map_size(map2));

  user_data found;
  int rc;

  rc = gfc_map_get(map1, "hello", &found);
  assert(GFC_ERROR_MAP_OK == rc);
  printf("hello = %s\n", (char*) found);

  rc = gfc_map_get(map1, "world", &found);
  assert(GFC_ERROR_MAP_OK == rc);
  printf("world = %s\n", (char*) found);

  gfc_map_deep_free(map1);
  gfc_map_deep_free(map2);

  map1 = gfc_map_new();

  for (int i = 0; i < 1000000; i++)
  {
    char idx[1024];
    sprintf(idx, "%08d", i);
    gfc_map_put(map1, idx, strdup(idx));
  }

  printf("size = %d\n", gfc_map_size(map1));
  return 0;
}
