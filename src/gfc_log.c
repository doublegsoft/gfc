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

/*!
** @file gfc_log.h
**
** @author 甘果 <guo.guo.gan@gmail.com>
**
** @date 2018-08-04
*/
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gfc_type.h"

/*!
** log file directory.
*/
static char               _gfc_log_dir[1024] = {'\0'};

/*!
** log file name.
*/
static char               _gfc_log_app[1024] = {'\0'};

static FILE*              _gfc_log_file = NULL;
static pthread_mutex_t    _gfc_log_lock;

static int                _gfc_log_year = 0;
static int                _gfc_log_month = 0;
static int                _gfc_log_day = 0;
static int                _gfc_log_hour = 0;
static int                _gfc_log_min = 0;

enum
{
  GFC_LOG_INFO    = 1,
  GFC_LOG_DEBUG   = 2,
  GFC_LOG_WARN    = 3,
  GFC_LOG_ERROR   = 4
};

static void
gfc_log_open()
{
  pthread_mutex_lock(&_gfc_log_lock);

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  int year = tm.tm_year + 1900;
  int month = tm.tm_mon + 1;
  int day = tm.tm_mday;
  int hour = tm.tm_hour;
  int min = tm.tm_min;

  char filepath[2048] = {'\0'};
  strcpy(filepath, _gfc_log_dir);
  strcat(filepath, "/");
  strcat(filepath, _gfc_log_app);
  strcat(filepath, ".log");

  if (_gfc_log_year == 0)
  {
    if (access(filepath, F_OK) == 0)
    {
      // already exists
      struct stat attrib;
      stat(filepath, &attrib);
      char date[10];
      tm = *gmtime(&(attrib.st_ctime));
      _gfc_log_year = tm.tm_year + 1900;
      _gfc_log_month = tm.tm_mon + 1;
      _gfc_log_day = tm.tm_mday;
      _gfc_log_hour = tm.tm_hour + 8 /* timezone */;
      _gfc_log_min = tm.tm_min;
    }
    else
    {
      _gfc_log_year = year;
      _gfc_log_month = month;
      _gfc_log_day = day;
      _gfc_log_hour = hour;
      _gfc_log_min = min;
    }
  }

  if (_gfc_log_year != year ||
      _gfc_log_month != month ||
      _gfc_log_day != day /*||
      _gfc_log_hour != hour ||
      _gfc_log_min != min*/)
  {
    char backup_filepath[2048] = {'\0'};

    sprintf(backup_filepath, "%s/%s-%d%02d%02d.log", _gfc_log_dir,
                                                     _gfc_log_app,
                                                     _gfc_log_year,
                                                     _gfc_log_month,
                                                     _gfc_log_day/*,
                                                     _gfc_log_hour,
                                                     _gfc_log_min*/);

    if (_gfc_log_file != NULL)
    {
      fclose(_gfc_log_file);
      _gfc_log_file = NULL;
    }

    rename(filepath, backup_filepath);

    _gfc_log_year = year;
    _gfc_log_month = month;
    _gfc_log_day = day;
    _gfc_log_hour = hour;
    _gfc_log_min = min;

  }

  if (_gfc_log_file == NULL)
    _gfc_log_file = fopen(filepath, "a+");

  pthread_mutex_unlock(&_gfc_log_lock);
}



/*!
** Initializes a log system, and can be used before close it.
**
** @param filepath
**        the log file path
**
** @return the return code, see gfc_types.h
*/
GFC_API void
gfc_log_init(const char* path, const char* app)
{
  DIR* dir = opendir(path);
  if (dir) {
    closedir(dir);
  } else {
#ifdef __MINGW32__
    mkdir(path);
#else
    mkdir(path, 0755);
#endif
  }
  strcpy(_gfc_log_dir, path);
  strcpy(_gfc_log_app, app);

  pthread_mutex_init(&_gfc_log_lock, NULL);
}

GFC_API void
gfc_log_close()
{
  fclose(_gfc_log_file);
}

GFC_API size_t
gfc_log_size()
{
  size_t ret;
  fseek(_gfc_log_file, 0L, SEEK_END);
  ret = ftell(_gfc_log_file);
  fseek(_gfc_log_file, 0L, SEEK_SET);
  return ret;
}

GFC_API void
gfc_log_info(const char *format, ...)
{
  if (format == NULL)
    return;

  gfc_log_open();

  va_list ap;
  va_start(ap, format);

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  fprintf(_gfc_log_file, "[INFO] %d-%02d-%02d %02d:%02d:%02d - ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  vfprintf(_gfc_log_file, format, ap);
  fprintf(_gfc_log_file, "\n");
  fflush(_gfc_log_file);

  va_end(ap);
}

GFC_API void
gfc_log_warn(const char *format, ...)
{
  if (format == NULL)
    return;

  gfc_log_open();

  va_list ap;
  va_start(ap, format);

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  fprintf(_gfc_log_file, "[WARN] %d-%02d-%02d %02d:%02d:%02d - ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  vfprintf(_gfc_log_file, format, ap);
  fprintf(_gfc_log_file, "\n");
  fflush(_gfc_log_file);

  va_end(ap);
}

GFC_API void
gfc_log_debug(const char *format, ...)
{
  if (format == NULL)
    return;

  gfc_log_open();

  va_list ap;
  va_start(ap, format);

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  fprintf(_gfc_log_file, "[DEBUG] %d-%02d-%02d %02d:%02d:%02d - ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  vfprintf(_gfc_log_file, format, ap);
  fprintf(_gfc_log_file, "\n");
  fflush(_gfc_log_file);

  va_end(ap);
}

GFC_API void
gfc_log_error(const char *format, ...)
{
  if (format == NULL)
    return;

  gfc_log_open();

  va_list ap;
  va_start(ap, format);

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  fprintf(_gfc_log_file, "[ERROR] %d-%02d-%02d %02d:%02d:%02d - ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  vfprintf(_gfc_log_file, format, ap);
  fprintf(_gfc_log_file, "\n");
  fflush(_gfc_log_file);

  va_end(ap);
}
