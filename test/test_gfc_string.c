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
#include <assert.h>

#include "gfc.h"


int
main()
{
  gfc_gc_init();
  gfc_string_p str = gfc_string_new("anybody");

  gfc_string_p s1 = gfc_string_new("s1");
  gfc_string_p s2 = gfc_string_new("s2");
  gfc_string_p s3 = gfc_string_new("s3");

  gfc_string_free(s1);
  gfc_string_free(s2);
  gfc_string_free(s3);

  int i = 0;
  gfc_string_concat(str, " ");
  gfc_string_concat(str, "hello");
  gfc_string_concat(str, " ");
  gfc_string_concat(str, "world");
  gfc_string_print(str);
  for (i = 0; i < 10; i++)
  {
    char utf8[1024] = {'\0'};
    strcpy(utf8, "中文转码测试，UTF8到GBK，再到UTF8，看一看是否出现缺少字节的情况。");

    char gbk[1024] = {'\0'};

    gfc_string_gbk(utf8, gbk, 1024);
    gfc_string_utf8(gbk, utf8, 1024);

    FILE* f = fopen("./test_gfc_string_output", "a+");
    fprintf(f, "%s", utf8);
    fprintf(f, "\n");
    fclose(f);

    gfc_string_concat(str, utf8);
  }

  gfc_string_print(str);
  printf("gfc_string_length(str) = %ld\n", gfc_string_length(str));
  printf("strlen(str->buffer) = %ld\n", strlen(str->buffer));

  gfc_string_free(str);
  printf("%lu bytes\n", gfc_gc_total());
  assert(gfc_gc_total() == 0);

  char string[50] = "Hello! We are learning about strtok";
  printf("splitting:\n");
  gfc_list_p list = gfc_string_split(string, " ");

  for (int i = 0; i < gfc_list_size(list); i++)
  {
    char* str = gfc_list_get(list, i);
    printf("%s\n", str);
  }
  gfc_list_deep_free(list);
  return 0;
}
