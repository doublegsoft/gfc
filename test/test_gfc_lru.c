

#include <gfc.h>

int main()
{
  gfc_lru_p lru = gfc_lru_new(1000, 1000);
  gfc_lru_set(lru, (void*)"hello", 5, "world", 5);

  void* val;
  gfc_lru_get(lru, "hello", 5, &val);

  printf("hello = %s\n", (char*) val);
  gfc_lru_free(lru);
  return 0;
}
