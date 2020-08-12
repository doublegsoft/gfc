
#include <stdio.h>

#include "gfc.h"


int
main()
{
  gfc_ring_p ring = gfc_ring_new(1000);

  int i = 0;
  int data[100];
  for (i = 0; i < 100; i++)
  {
    data[i] = i;
    gfc_ring_push(ring, &data[i]);
  }

  int* pi;
  for (i = 0; i < 100; i++)
  {
    pi = gfc_ring_pop(ring);
    printf("pop: %d\n", *pi);
  }

  gfc_ring_clear(ring);
  gfc_ring_free(ring);
  return 0;
}
