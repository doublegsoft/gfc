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
** @file gfc_net.h
**
** @author Christian Gann <guo.guo.gan@gmail.com>
**
** @date 2019-03-21
**
** @since 1.0
*/

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define WIN32_LEAN_AND_MEAN

#else

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#endif

#include <string.h>
#include <strings.h>

#include "gfc_tcp.h"
#include "gfc_type.h"

GFC_API int
gfc_tcp_sock()
{
  int ret = -1;
#ifdef _WIN32
  WSADATA wsadata;
  WSAStartup(MAKEWORD(1, 1), &wsadata);
  ret = socket(PF_INET, SOCK_STREAM, 0);
#else
  ret = socket(AF_INET, SOCK_STREAM, 0);
#endif

  return ret;
}

GFC_API int
gfc_tcp_connect(int sock, const char* hostname, int port)
{
  int rc;
//  if ((rc = fcntl(sock, F_GETFL, NULL)) < 0)
//    return -100;
//
//  // set socket non-blocking
//  if (fcntl(sock, F_SETFL, rc | O_NONBLOCK) < 0)
//    return -100;

  struct sockaddr_in    serv_addr;
  struct hostent*       server;

  server = gethostbyname(hostname);
  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy((char *) &serv_addr.sin_addr.s_addr,
         (char *) server->h_addr,
         server->h_length);
  serv_addr.sin_port = htons(port);

  return connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
}
