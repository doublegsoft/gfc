
#include <unistd.h>

#include "gfc.h"

#define TEST_THREAD_NUMBER                8

#include <stdio.h>

struct heapsort_data_s
{

  int* nums;

  int len;

  int start;

  int end;

};

void swap(int *a, int *b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
}

void heapify(int arr[], int n, int i)
{
  int largest = i;
  int left = 2 * i + 1;
  int right = 2 * i + 2;

  if (left < n && arr[left] > arr[largest])
    largest = left;

  if (right < n && arr[right] > arr[largest])
    largest = right;

  if (largest != i)
  {
    swap(&arr[i], &arr[largest]);
    heapify(arr, n, largest);
  }
}

void heap_sort(int arr[], int start, int end)
{
  int n = end - start;
  for (int i = n / 2 - 1; i >= 0; i--)
    heapify(arr, n, i);

  for (int i = n - 1; i >= 0; i--)
  {
    swap(&arr[0], &arr[i]);
    heapify(arr, i, 0);
  }
}


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

static void*
heapsort_worker(void* data)
{
  struct heapsort_data_s* p = (struct heapsort_data_s*) data;
  heap_sort(p->nums, p->start, p->end);

  return NULL;
}

void
test_individual_threads()
{
  gfc_mt_p threads = gfc_mt_new(TEST_THREAD_NUMBER / 2);

  int i = 0;
  int data[TEST_THREAD_NUMBER];
  for (i = 0; i < TEST_THREAD_NUMBER; i++)
  {
    data[i] = 20;
    gfc_mt_do(threads, fibonacci_worker, &data[i]);
  }
  gfc_mt_free(threads);
}

void
test_collaborative_thread()
{
  int count = 2048 * 20;
  int numbers[count];
  int i = 0;

  gfc_mt_p threads = gfc_mt_new(10);

  struct heapsort_data_s data[TEST_THREAD_NUMBER];
  for (i = 0; i < TEST_THREAD_NUMBER; i++)
  {
    data[i].nums = numbers;
    data[i].len = count;
    data[i].start = i * 2048;
    data[i].end = (i + 1) * 2048;
    gfc_mt_do(threads, heapsort_worker, &data[i]);
  }
  gfc_mt_free(threads);
}

int
main()
{
  gfc_gc_init();

  test_individual_threads();
  test_collaborative_thread();

  return 0;
}
