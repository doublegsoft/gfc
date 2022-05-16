/*
** gfc
**
** Copyright (C) 2019 doubelgsoft.open
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
** along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*!
** @file gfc_log.h
**
** @author 甘果 <guo.guo.gan@gmail.com>
**
** @date 2018-08-04
*/
#ifndef __GFC_LOG_H__
#define __GFC_LOG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdarg.h>

static FILE *logfile;

enum
{
  GFC_LOG_INFO    = 1,
  GFC_LOG_DEBUG   = 2,
  GFC_LOG_WARN    = 3,
  GFC_LOG_ERROR   = 4
};

/*!
** Initializes a log system, and can be used before close it.
**
** @param filepath
**        the log file path
**
** @return the return code, see gfc_types.h
*/
GFC_API int
gfc_log_init(const char* path, const char* app);

GFC_API int
gfc_log_close();

GFC_API size_t
gfc_log_size();

GFC_API void
gfc_log_info(const char *format, ...);

GFC_API void
gfc_log_warn(const char *format, ...);

GFC_API void
gfc_log_debug(const char *format, ...);

GFC_API void
gfc_log_error(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // __GFC_LOG_H__
