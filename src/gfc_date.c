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
#include <time.h>
#include <stdio.h>

#include "gfc_date.h"
#include "gfc_type.h"

void
gfc_date_stringify(char* buff, struct tm* timeinfo, gfc_date_field_e field)
{
  int year = timeinfo->tm_year + 1900;
  int month = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;
  int min = timeinfo->tm_min;
  int sec = timeinfo->tm_sec;
  switch (field)
  {
    case YEAR:
      sprintf(buff, "%d", year);
      break;
    case MONTH:
      sprintf(buff, "%d-%02d", year, month);
      break;
    case DAY:
      sprintf(buff, "%d-%02d-%02d", year, month, day);
      break;
    case HOUR:
      sprintf(buff, "%d-%02d-%02d %02d", year, month, day, hour);
      break;
    case MINUTE:
      sprintf(buff, "%d-%02d-%02d %02d:%02d", year, month, day, hour, min);
      break;
    case SECOND:
      sprintf(buff, "%d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, sec);
      break;
    default:
      break;
  }
}

void
gfc_date_compact(char* buff, struct tm* timeinfo, gfc_date_field_e field)
{
  int year = timeinfo->tm_year + 1900;
  int month = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;
  int min = timeinfo->tm_min;
  int sec = timeinfo->tm_sec;
  switch (field)
  {
    case YEAR:
      sprintf(buff, "%d", year);
      break;
    case MONTH:
      sprintf(buff, "%d%02d", year, month);
      break;
    case DAY:
      sprintf(buff, "%d%02d%02d", year, month, day);
      break;
    case HOUR:
      sprintf(buff, "%d%02d%02d%02d", year, month, day, hour);
      break;
    case MINUTE:
      sprintf(buff, "%d%02d%02d%02d%02d", year, month, day, hour, min);
      break;
    case SECOND:
      sprintf(buff, "%d%02d%02d%02d%02d%02d", year, month, day, hour, min, sec);
      break;
    default:
      break;
  }
}

int
gfc_date_now(char* buff)
{
  time_t rawtime;
  struct tm* timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  gfc_date_stringify(buff, timeinfo, SECOND);

  return GFC_SUCCESS;
}

int
gfc_date_compactday(char* buff)
{
  time_t rawtime;
  struct tm* timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  gfc_date_compact(buff, timeinfo, DAY);

  return GFC_SUCCESS;
}

int
gfc_date_add(char* buff, time_t raw, int secs)
{
  raw += secs;
  struct tm* timeinfo;
  timeinfo = localtime (&raw);

  gfc_date_stringify(buff, timeinfo, SECOND);
  return GFC_SUCCESS;
}

