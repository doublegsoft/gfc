/*
 * gfc
 *
 * Copyright (C) 2019 doublegsoft.open
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GFC_TYPE_H__
#define __GFC_TYPE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef  _WIN32
#define GFC_API   __declspec(dllexport)
#else
#define GFC_API
#endif

typedef unsigned char                   byte;
typedef unsigned int                    uint;
typedef unsigned long                   ulong;

#define GFC_BUFFER_SIZE                 4096
#define GFC_MAX_PATH                    8192

#define GFC_SUCCESS                     0
#define GFC_OK                          0

#define GFC_ERROR_FILE_OPEN             0101
#define GFC_ERROR_FILE_WRITE            0102
#define GFC_ERROR_FILE_READ             0103

#define GFC_ERROR_SOCK_INIT             0201
#define GFC_ERROR_SOCK_BIND             0202
#define GFC_ERROR_SOCK_SEND             0203
#define GFC_ERROR_SOCK_RECV             0204

#define GFC_ERROR_OBJECT_NULL           0304

#define GFC_ERROR_SOCKET_INVALID        (uint)(~0)
#define GFC_ERROR_SOCKET_BIND           0401
#define GFC_ERROR_SOCKET_LISTEN         0402

#define GFC_ERROR_IOCP_CREATE           0501

#define GFC_ERROR_SYSINFO_GET           0601

#define GFC_ERROR_INIT                  0001

#ifndef NULL
#define NULL                            0
#endif

typedef void*                           user_data;

#ifdef __cplusplus
}
#endif

#endif // __GFC_TYPE_H__
