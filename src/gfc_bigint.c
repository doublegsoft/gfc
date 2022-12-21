/*
 *           .d888
 *          d88P"
 *          888
 *  .d88b.  888888 .d8888b
 * d88P"88b 888   d88P"
 * 888  888 888   888
 * Y88b 888 888   Y88b.
 *  "Y88888 888    "Y8888P
 *      888
 * Y8b d88P
 *  "Y88P"
 */

#include <stdio.h>

#include "gfc_bigint.h"

typedef struct gfc_bigint_s
{
  unsigned char* digits;

  unsigned int num_digits;

  unsigned int num_allocated_digits;

  int is_negative;
}
gfc_bigint_t;

gfc_bigint_p
gfc_bigint_new(const char* val)
{
  gfc_bigint_p ret = (gfc_bigint_p) malloc(sizeof(gfc_bigint_t));

  assert(ret != NULL);

  return ret;
}

void
gfc_bigint_free(gfc_bigint_p bigint)
{
  free(bigint);
}
