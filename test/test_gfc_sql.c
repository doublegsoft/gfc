
#include <stdio.h>

#include "gfc.h"


int
main()
{
  gfc_sql_set("1", "{{a}} + {{b}} = ?");

  char* res;

  gfc_sql_get("1", "{\"a\":1,\"b\":\"2\"}", &res);

  printf("result = %s\n", res);

  free(res);

  return 0;
}
