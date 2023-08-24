
#include <stdio.h>

#include "gfc.h"


int
main()
{
  gfc_gc_init();
  gfc_string_p str = gfc_string_new("anybody");

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
    fprintf(f, utf8);
    fprintf(f, "\n");
    fclose(f);

    gfc_string_concat(str, utf8);
  }

  gfc_string_print(str);
  printf("gfc_string_length(str) = %ld\n", gfc_string_length(str));
  printf("strlen(str->buffer) = %ld\n", strlen(str->buffer));

  return 0;
}
