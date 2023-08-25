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
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <inttypes.h>
#include <unistd.h>

#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef unsigned long in_addr_t;
#endif

#include "gfc_ws.h"

#define GFC_WS_REQUEST_DUMMY            "GET / HTTP/1.1\r\n"                                      \
                                        "Host: localhost:8080\r\n"                                \
                                        "Connection: Upgrade\r\n"                                 \
                                        "Upgrade: websocket\r\n"                                  \
                                        "Sec-WebSocket-Version: 13\r\n"                           \
                                        "Sec-WebSocket-Key: uaGPoPbZRzHcWDXiNQ5dyg==\r\n\r\n"

int
gfc_ws_conn(gfc_ws_p ctx, const char *ip, uint16_t port)
{
  char *p;
  int sock;
  ssize_t ret;
  in_addr_t ip_addr;
  struct sockaddr_in sock_addr;

  memset(ctx, 0, sizeof(*ctx));

#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    fprintf(stderr, "WSAStartup failed!");
    return (-1);
  }

  /**
   * Sets stdout to be non-buffered.
   *
   * According to the docs from MSDN (setvbuf page), Windows do not
   * really supports line buffering but full-buffering instead.
   *
   * Quote from the docs:
   * "... _IOLBF For some systems, this provides line buffering.
   *  However, for Win32, the behavior is the same as _IOFBF"
   */
  setvbuf(stdout, NULL, _IONBF, 0);
#endif

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    return (-1);

  memset((void*)&sock_addr, 0, sizeof(sock_addr));
  sock_addr.sin_family = AF_INET;

  if ((ip_addr = inet_addr(ip)) == INADDR_NONE)
    return (-1);

  sock_addr.sin_addr.s_addr = ip_addr;
  sock_addr.sin_port = htons(port);

  /* Connect. */
  if (connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
    return errno;

  /* Do handhshake. */
  if (send(sock, GFC_WS_REQUEST_DUMMY, strlen(GFC_WS_REQUEST_DUMMY), MSG_NOSIGNAL) < 0)
    return errno;

  /* Wait for 'switching protocols'. */
  if ((ret = recv(sock, ctx->frm, sizeof(ctx->frm), 0)) < 0)
    return errno;

  /* Advance our pointers before the first next_byte(). */
  p = strstr((const char *)ctx->frm, "\r\n\r\n");
  ctx->amt_read = ret;
  ctx->cur_pos  = (size_t)((ptrdiff_t)(p - (char *)ctx->frm)) + 4;
  ctx->fd       = sock;
  ctx->status   = TWS_ST_CONNECTED;

  return GFC_WS_OK;
}
