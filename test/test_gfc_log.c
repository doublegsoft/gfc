
#include <stdio.h>
#include <unistd.h>

#include "gfc.h"


int
main()
{
  gfc_log_init("log", "test");

  int i = 0;

  for (i = 0; i < 1000; i++)
  {
    gfc_log_info("%s-%s", "hello", "world");
  }

  sleep(60);

  for (i = 0; i < 1000; i++)
  {
    gfc_log_info("%s-%s", "hello", "world");
  }

  sleep(60);

  for (i = 0; i < 1000; i++)
  {
    gfc_log_info("%s-%s", "hello", "world");
  }

  sleep(60);

  for (i = 0; i < 1000; i++)
  {
    gfc_log_info("%s-%s", "hello", "world");
  }

  return 0;
}
