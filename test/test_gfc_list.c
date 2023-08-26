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
#include <assert.h>

#include "gfc.h"

int descend(int* a, int* b) {
  int ret = 1;
  if (*a > *b) ret = -1;
  if (*a == *b) ret = 0;
  return ret;
}

int
main()
{
  gfc_gc_init();

  gfc_list_p list = gfc_list_new();
  int i;
  int a = 1;
  int b = 2;
  int c = 3;
  gfc_list_append(list, &a);
  gfc_list_append(list, &b);
  gfc_list_append(list, &c);

  int* pa = gfc_list_get(list, 0);
  int* pb = gfc_list_get(list, 1);
  int* pc = gfc_list_get(list, 2);

  printf("list size = %d\n", gfc_list_size(list));
  printf("0 = %d\n", *pa);
  printf("1 = %d\n", *pb);
  printf("2 = %d\n", *pc);

  // remove middle (value is 2)
  gfc_list_remove(list, 1);

  pa = gfc_list_get(list, 0);
  pb = gfc_list_get(list, 1);

  printf("list size = %d\n", gfc_list_size(list));
  printf("0 = %d\n", *pa);
  printf("1 = %d\n", *pb);

  // remove last (value is 3)
  gfc_list_remove(list, 1);

  pa = gfc_list_get(list, 0);

  printf("list size = %d\n", gfc_list_size(list));
  printf("0 = %d\n", *pa);

  gfc_list_free(list);

  assert(gfc_gc_total() == 0);

  int MAX = 30000;
  list = gfc_list_new();
  for (i = 0; i < MAX; i++)
  {
    int* a = (int*)gfc_gc_malloc(sizeof(int), 1);
//    int* a = (int*)malloc(sizeof(int));
    *a = i;
    gfc_list_append(list, a);
  }

  printf("%lu blocks used memory: %lu\n", gfc_gc_length(), gfc_gc_total());

  for (i = 0; i < MAX; i++)
    gfc_gc_free(gfc_list_get(list, i));

  gfc_list_free(list);

  printf("used memory after free: %lu\n", gfc_gc_total());
  assert(gfc_gc_total() == 0);


//  gfc_list_sort(list, descend);
//
//  int* pval = 0;
//  for (i = 0; i < 30000; i++)
//  {
//    pval = gfc_list_get(list, i);
//  }
//  printf("the val = %d\n", *pval);


  return 0;
}
