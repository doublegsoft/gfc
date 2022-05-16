
#include <stdio.h>

#include "gfc.h"

int descend(int* a, int* b) {
  int ret = 1;
  if (*a > *b) ret = -1;
  if (*a == *b) ret = 0;
  return ret;
}

int
main()
{
  gfc_list_p list = gfc_list_new();
  int i;
  int a = 1;
  int b = 2;
  int c = 3;
  gfc_list_append(list, &a);
  gfc_list_append(list, &b);
  gfc_list_append(list, &c);

  int* pa = gfc_list_get(list, 0);
  int* pb = gfc_list_get(list, 1);
  int* pc = gfc_list_get(list, 2);

  printf("list size = %d\n", gfc_list_size(list));
  printf("0 = %d\n", *pa);
  printf("1 = %d\n", *pb);
  printf("2 = %d\n", *pc);

  // remove middel
  gfc_list_remove(list, 1);

  pa = gfc_list_get(list, 0);
  pb = gfc_list_get(list, 1);

  printf("list size = %d\n", gfc_list_size(list));
  printf("0 = %d\n", *pa);
  printf("1 = %d\n", *pb);

  // remove last
  gfc_list_remove(list, 1);
  pa = gfc_list_get(list, 0);

  printf("list size = %d\n", gfc_list_size(list));
  printf("0 = %d\n", *pa);

  gfc_list_free(list);

  list = gfc_list_new();
  for (i = 0; i < 100000; i++)
  {
    int* a = (int*)malloc(sizeof(int));
    *a = i;
    gfc_list_append(list, a);
  }

  gfc_list_sort(list, descend);

  int* pval = 0;
  for (i = 0; i < 100000; i++)
  {
    pval = gfc_list_get(list, i);
  }
  printf("the val = %d\n", *pval);


  return 0;
}
