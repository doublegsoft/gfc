/*
** gfc
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

#ifndef __GFC_SQL_H__
#define __GFC_SQL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define GFC_ERROR_SQL_OK            0
#define GFC_ERROR_SQL_NOT_FOUND     4001

int
gfc_sql_set(const char* id, const char* sql);

int
gfc_sql_get(const char* id, const char* json, char** sql);


#ifdef __cplusplus
}
#endif

#endif // __GFC_SQL_H__
