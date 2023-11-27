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
#include <gfc.h>

int main()
{
  gfc_gc_init();
  gfc_lru_p lru = gfc_lru_new(1000, 1000);
  gfc_lru_set(lru, (void*)"hello", 5, "world", 5);

  void* val;
  gfc_lru_get(lru, "hello", 5, &val);

  printf("hello = %s\n", (char*) val);

  gfc_lru_remove(lru, (void*)"hello", 5);
  gfc_lru_remove(lru, (void*)"hello", 5);

  gfc_lru_free(lru);
  return 0;
}
