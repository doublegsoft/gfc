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
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include "gfc.h"

static char sender[1024] = {'\0'};
static char receiver[1024] = {'\0'};

static char*
get_message_content(char* msg)
{
  char* ptr = strstr(msg, "messageContent");
  if (ptr == NULL)
    return NULL;
  ptr += 16;

  int i = 0;
  char* start = NULL;
  char* end = NULL;
  while (1)
  {
    if (*ptr == '"')
    {
      if (start == NULL)
        start = ptr;
      else
      {
        end = ptr;
        break;
      }
    }
    ptr++;
  }
  char* ret = gfc_gc_malloc(sizeof(char), end - start + 1);
  int index = 0;
  for (ptr = start + 1; ptr != end; ptr++)
    ret[index++] = *ptr;
  return ret;
}

static void*
gfc_ws_test_recv(void* data)
{
  printf("接收线程开始运行...\n");
  gfc_ws_p ctx = (gfc_ws_p) data;
  while (1)
  {
    char *buff;
    int frm_type;
    size_t buff_size;
    gfc_ws_recv(ctx, &buff, &buff_size, &frm_type);
    if (buff == NULL)
    {
      sleep(1);
      continue;
    }
    char* msg = get_message_content(buff);
    if (msg != NULL)
    {
      printf("\t\t\t\t%s\n", msg);
      gfc_gc_free(msg);
    }
  }
  return NULL;
}

static void*
gfc_ws_test_send(void* data)
{
  printf("发送线程开始运行...\n");
  gfc_ws_p ctx = (gfc_ws_p) data;
  char msg[4096] = {'\0'};

  while (1)
  {
    char* buff;
    int frm_type;
    size_t buff_size;
    char msg[4096] = {'\0'};
    sprintf(msg,
            "{"
            "\"operation\":\"sendMessage\","
            "\"userId\":\"%s\","
            "\"userType\":\"TEST\","
            "\"payload\":{"
            "\"receiverId\":\"%s\","
            "\"receiverType\":\"TEST\","
            "\"receiverAlias\":\"%s号选手\","
            "\"messageType\": \"TEXT\","
            "\"messageContent\": \"你好, %s号\""
            "}"
            "}",
            sender,
            receiver,
            receiver,
            receiver);
    assert(GFC_WS_OK == gfc_ws_send(ctx, (uint8_t *) msg, strlen(msg), FRM_TXT));
    printf("你好, %s号\n", receiver);
    sleep(3);
  }
}

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    printf("usage:\n");
    printf("\ttest_gfc_ws <用户标识>\n");
    return 1;
  }
  gfc_gc_init();

  strcpy(sender, argv[1]);

  if (strcmp(sender, "1") == 0)
    strcpy(receiver, "2");
  if (strcmp(sender, "2") == 0)
    strcpy(receiver, "1");
  if (strcmp(sender, "3") == 0)
      strcpy(receiver, "4");
  if (strcmp(sender, "4") == 0)
      strcpy(receiver, "3");

  gfc_ws_t ctx;

  assert(GFC_WS_OK == gfc_ws_conn(&ctx, "139.186.131.138", 5678));
//  assert(GFC_WS_OK == gfc_ws_conn(&ctx, "127.0.0.1", 9999));

  char msg[4096] = {'\0'};
  sprintf(msg,
          "{"
          "\"operation\":\"login\","
          "\"userId\":\"%s\","
          "\"userType\":\"TEST\","
          "\"payload\":{"
          "}"
          "}",
          sender);
  assert(GFC_WS_OK == gfc_ws_send(&ctx, (uint8_t *) msg, strlen(msg), FRM_TXT));

  sleep(1);

  pthread_t thread_sender;
  pthread_t thread_receiver;

  pthread_create(&thread_sender, NULL, gfc_ws_test_send, (void*) &ctx);
  pthread_create(&thread_receiver, NULL, gfc_ws_test_recv, (void*) &ctx);
  pthread_join(thread_sender, NULL);
  pthread_join(thread_receiver, NULL);

  return 0;
}
