
#include <unistd.h>

#include "gfc.h"

void*
thread_entry(void* data)
{
  int* id = (int*) data;
  int i = 0;
  for (i = 0; i < 3; i++)
  {
    printf("%d -- %d\n", *id, i);
    sleep(1);
  }
  return NULL;
}

int
main()
{
  gfc_threads_p threads = gfc_threads_new(10);

  int i = 0;
  int data[20];
  for (i = 0; i < 20; i++)
  {
    data[i] = i;
    gfc_threads_do(threads, thread_entry, &data[i]);
  }

  gfc_threads_free(threads);
  return 0;
}
