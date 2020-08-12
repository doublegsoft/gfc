
#include <stdio.h>

#include "gfc.h"


int
main()
{
  gfc_string_p str = gfc_string_new("anybody", 7);

  int i = 0;
  gfc_string_concat(str, " ", 1);
  gfc_string_concat(str, "hello", 5);
  gfc_string_concat(str, " ", 1);
  gfc_string_concat(str, "world", 5);
  for (i = 0; i < 0; i++)
  {
    const char* utf8 = "中文来了";
    char gbk[128] = {'\0'};
    gfc_string_gbk(utf8, gbk, 128);
    gfc_string_concat(str, utf8, strlen(utf8));
  }

  gfc_string_print(str);
  printf("gfc_string_length(str) = %I64u\n", gfc_string_length(str));
  printf("strlen(str->buffer) = %I64u\n", strlen(str->buffer));

  return 0;
}
