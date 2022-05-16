#include <stdio.h>

#include "gfc.h"


int
main()
{
  int fd = gfc_tcp_sock();
  int rc = gfc_tcp_connect(fd, "10.6.2.20", 8888);

  printf("connection result = %d\n", rc);
  return 0;
}
