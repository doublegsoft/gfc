
#include <stdio.h>

#include "gfc.h"


int
main()
{
  gfc_map_p map = gfc_map_new();

  gfc_map_put(map, "hello", "world");
  gfc_map_put(map, "world", "hello");

  user_data data;

  gfc_map_get(map, "hello", &data);
  printf("hello = %s\n", (char*) data);
  gfc_map_get(map, "world", &data);
  printf("world = %s\n", (char*) data);

  return 0;
}
