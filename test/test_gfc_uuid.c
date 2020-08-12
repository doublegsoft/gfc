
#include <stdio.h>

#include "gfc.h"


int
main()
{
  char str[37] = {'\0'};
  gfc_uuid_t uuid;
  gfc_uuid_generate(uuid);
  gfc_uuid_upper(uuid, str);

  printf("uuid = %s\n", str);

  return 0;
}
