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
#ifndef __GFC_WS_H__
#define __GFC_WS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>


#include "gfc_type.h"

#ifdef __APPLE__
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
#endif

#define GFC_WS_OK                         0

#define FRM_TXT                           1
#define FRM_BIN                           2
#define FRM_CLSE                          8
#define FRM_FIN                           128
#define FRM_MSK                           128

#ifndef MESSAGE_LENGTH
#define MESSAGE_LENGTH                    2048
#endif

/* Client status. */
#define TWS_ST_DISCONNECTED               0
#define TWS_ST_CONNECTED                  1

typedef struct gfc_ws_s
{
  uint8_t frm[MESSAGE_LENGTH];
  size_t amt_read;
  size_t cur_pos;
  int status;
  int fd;
}
gfc_ws_t;

typedef gfc_ws_t*                     gfc_ws_p;

/*!
** initializes web socket client context.
*/
GFC_API int
gfc_ws_conn(gfc_ws_p ctx, const char* ip, uint16_t port);

/*!
** sends message to server.
*/
GFC_API int
gfc_ws_send(gfc_ws_p ctx, uint8_t* msg, uint64_t size, int type);

/*!
** receives message from server.
*/
GFC_API int
gfc_ws_recv(gfc_ws_p ctx, char** buff, size_t* buff_size, int* frm_type);

/*!
** closes web socket client context.
*/
GFC_API void
gfc_ws_close(gfc_ws_p ctx);

#ifdef __cplusplus
};
#endif

#endif // __GFC_WS_H__
