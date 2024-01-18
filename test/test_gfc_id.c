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

int
main()
{
  gfc_id_t ctx = {
    .last_timestamp = 0,
    .datacenter = 1,
    .machine = 1,
    .seq = 0
  };
  gfc_id_init(&ctx);

  char buff[64];
  for (int i = 0; i < 10; i++)
  {
    gfc_id_string(buff);
    printf("%d: %s\n", i + 1, buff);
  }

  return GFC_GC_OK;
}
