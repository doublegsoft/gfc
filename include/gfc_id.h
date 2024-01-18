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
#ifndef __GFC_ID_H__
#define __GFC_ID_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "gfc_type.h"

#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint64_t last_timestamp;
  int datacenter;
  int machine;
  int seq;
}
gfc_id_t;

GFC_API void
gfc_id_init(gfc_id_t*);

GFC_API uint64_t
gfc_id_value();

GFC_API void
gfc_id_string(char* buff);

#ifdef __cplusplus
}
#endif

#endif // __GFC_ID_H__
