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
** Copyright (C) 2020 doublegsoft.open
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

#include "gfc_sql.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "gfc_map.h"



static gfc_map_p _gfc_sqls = NULL;

GFC_API int
gfc_sql_set(const char* id, const char* sql)
{
  if (_gfc_sqls == NULL) _gfc_sqls = gfc_map_new();

  gfc_map_put(_gfc_sqls, id, (user_data)sql);

  return GFC_ERROR_SQL_OK;
}

GFC_API int
gfc_sql_get(const char* id, gfc_map_p params, char** sql)
{
  if (_gfc_sqls == NULL)
    return GFC_ERROR_SQL_NOT_FOUND;

  char* sqltpl = NULL;

  if (gfc_map_get(_gfc_sqls, id, (user_data*)&sqltpl) == GFC_ERROR_MAP_MISSING)
    return GFC_ERROR_SQL_NOT_FOUND;

  size_t size = 0;

//  json_object* params = json_tokener_parse(json);
//  int rc = mustach_json_c(sqltpl, params, sql, &size);

  return GFC_ERROR_SQL_OK;
}
