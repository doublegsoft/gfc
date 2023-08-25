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
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "gfc.h"

static void test_malloc_then_free()
{
  printf("test_malloc_then_free:\n");
  for (int i = 0; i < 10000; i++)
  {
    void* ptr = gfc_gc_malloc(1000, 1);
    int rc = gfc_gc_free(ptr);
    assert(rc == GFC_GC_OK);
  }

  printf("  memory allocated now: %lu\n", gfc_gc_total());
}

static void test_realloc_then_free()
{
  printf("test_remalloc_then_free:\n");

  for (int i = 0; i < 1000; i++)
  {
    void* ptr = gfc_gc_malloc(10, 1);
    ptr = gfc_gc_realloc(ptr, 10, 100);

    assert(gfc_gc_free(ptr) == GFC_GC_OK);
  }

  printf("  memory allocated now: %lu\n", gfc_gc_total());
}

int
main()
{
  gfc_gc_init();

  test_malloc_then_free();
  test_realloc_then_free();

  return GFC_GC_OK;
}
