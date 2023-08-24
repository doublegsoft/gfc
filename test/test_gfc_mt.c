
#include <unistd.h>

#include "gfc.h"

#define TEST_THREAD_NUMBER                8

static int fibonacci(int n) {
//  printf("%d\n", n);
  if (n <= 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  } else {
    return fibonacci(n - 1) + fibonacci(n - 2);
  }
}

static void*
fibonacci_worker(void* data)
{
  int* n = (int*) data;
  int v = *n;
  fibonacci(v);

  return NULL;
}

void
test_individual_threads()
{
  gfc_mt_p threads = gfc_mt_new(TEST_THREAD_NUMBER);

  int i = 0;
  int data[TEST_THREAD_NUMBER];
  for (i = 0; i < TEST_THREAD_NUMBER; i++)
  {
    data[i] = 20;
    gfc_mt_do(threads, fibonacci_worker, &data[i]);
  }
  gfc_mt_free(threads);
}

int
main()
{
  gfc_gc_init();

  test_individual_threads();

  return 0;
}
