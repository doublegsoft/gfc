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
#include "gfc_http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <stdio.h>
  #pragma comment(lib, "Ws2_32.lib")
#elif _LINUX
  #include <sys/socket.h>
#else
  #error Platform not supported.
#endif

#define GFC_FILE_BUFF_SIZE                      4096

int
gfc_http_upload(const char* hostname,
                const char* port,
                const char* uri,
                const char* paramname,
                const char* filepath,
                gfc_list_t* parameters)
{
  if (filepath == NULL)
    return -1;

  // 打开文件
  FILE *fp = fopen(filepath, "rb");
  if (fp == NULL)
    return -1;

#if defined WIN32
  // Initialize Winsock
  WSADATA wsaData;
  int res = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (res != 0)
    return -1;
#endif

  size_t  read_bytes = 0;
  char read_buff[GFC_FILE_BUFF_SIZE];

  struct addrinfo hints;
  struct addrinfo *server;

  memset(&hints, 0, sizeof(hints));
#if defined WIN32
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
#endif

  // 解析服务器地址
  getaddrinfo(hostname, port, &hints, &server);

  // 创建套接字
  int sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
  if (sockfd < 0)
    return -1;

  int status = connect(sockfd, server->ai_addr, server->ai_addrlen);
  if (status != 0)
    return -1;

  long content_length = 0;
  const char *filename = gfc_file_name(filepath);
  long filesize = gfc_file_size(filepath);

  char formdata[GFC_FILE_BUFF_SIZE] = {'\0'};

  if (parameters != NULL)
  {
    int i = 0;
    for (; i < parameters->size; i++)
    {
//      "\r\n--84989444e2484915a216e1718e0f93f0\r\n"
//      "Content-Disposition: form-data; name=\"username\"\r\n"
//      "\r\n%s"
      char *pair = (char *)gfc_list_get(parameters, i);
    }
  }

  const char *header_format = "POST %s HTTP/1.1\r\n"
                              "Host: %s\r\n"
                              "Content-Length: %ld\r\n"
                              "Content-Type: multipart/form-data; charset=GBK; boundary=84989444e2484915a216e1718e0f93f0\r\n"
                              "\r\n--84989444e2484915a216e1718e0f93f0\r\n"
                              "Content-Type: application/octet-stream\r\n"
                              "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n\r\n";
  const char *footer = "\r\n--84989444e2484915a216e1718e0f93f0--\r\n";
  char header[GFC_FILE_BUFF_SIZE] = {'\0'};
  sprintf(header, header_format, uri, hostname, filesize + 2048, paramname, filename);

  send(sockfd, header, strlen(header), 0);

  while ((read_bytes = fread(read_buff, sizeof(char), GFC_FILE_BUFF_SIZE, fp)) >= 1)
    send(sockfd, read_buff, read_bytes, 0);

  send(sockfd, footer, strlen(footer), 0);

  fclose(fp);

#ifdef WIN32
  closesocket(sockfd);
  WSACleanup();
#else _LINUX
  close(sockfd);
#endif

  return 0;
}
