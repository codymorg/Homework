#include <stdio.h>     /* printf, sprintf, fflush    */
#include <stdlib.h>    /*  malloc, free, abort, atoi */
#include "PRNG.h"      /* RandomInt, ut_srand        */
#include "cachelist.h" /* cachelist stuff            */

void SwapInt(int *a, int *b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
}

void Shuffle(int *array, int count)
{
  int i;
  for (i = 0; i < count; i++)
  {
    int r = RandomInt(0, count - 1);
    SwapInt(&array[i], &array[r]);
  }
}

void PrintArray(int *array, int count)
{
  int i;
  for (i = 0; i < count; i++)
    printf("%i  ", array[i]);
  printf("\n");
}

void test_add_front(void)
{
  cl_node *list = NULL;

  printf("----- test_add_front ----------------------------------------\n");
  list = cl_add_front(list, 1, "one");
  list = cl_add_front(list, 2, "two");
  list = cl_add_front(list, 3, "three");
  list = cl_add_front(list, 4, "four");
  list = cl_add_front(list, 5, "five");
  list = cl_add_front(list, 6, "six");
  list = cl_add_front(list, 7, "seven");

  cl_dump(list);
  cl_destroy(list);
}

void test_add_end(void)
{
  cl_node *list = NULL;

  printf("----- test_add_end ----------------------------------------\n");
  list = cl_add_end(list, 1, "one");
  list = cl_add_end(list, 2, "two");
  list = cl_add_end(list, 3, "three");
  list = cl_add_end(list, 4, "four");
  list = cl_add_end(list, 5, "five");
  list = cl_add_end(list, 6, "six");
  list = cl_add_end(list, 7, "seven");

  cl_dump(list);
  cl_destroy(list);
}

void test_remove1(void)
{
  cl_node *list = NULL;

  printf("----- test_remove1 ----------------------------------------\n");
  list = cl_add_end(list, 1, "one");
  list = cl_add_end(list, 2, "two");
  list = cl_add_end(list, 3, "three");
  list = cl_add_end(list, 4, "four");
  list = cl_add_end(list, 5, "five");
  list = cl_add_end(list, 6, "six");
  list = cl_add_end(list, 7, "seven");
  cl_dump(list);

  list = cl_remove(list, 4);
  cl_dump(list);

  list = cl_remove(list, 5);
  cl_dump(list);

  list = cl_remove(list, 1);
  cl_dump(list);

  list = cl_remove(list, 7);
  cl_dump(list);

  cl_destroy(list);
}


void test_remove2(void)
{
  cl_node *list = NULL;

  printf("----- test_add_remove2 ----------------------------------------\n");
  list = cl_add_end(list, 1, "one");
  list = cl_add_end(list, 2, "two");
  list = cl_add_end(list, 3, "three");
  list = cl_add_end(list, 4, "four");
  list = cl_add_end(list, 5, "five");
  list = cl_add_end(list, 6, "six");
  list = cl_add_end(list, 7, "seven");
  cl_dump(list);

  list = cl_remove(list, 99);
  cl_dump(list);

  list = cl_remove(list, 4);
  cl_dump(list);

  list = cl_remove(list, 5);
  cl_dump(list);

  list = cl_remove(list, 1);
  cl_dump(list);

  list = cl_remove(list, 7);
  cl_dump(list);

  list = cl_remove(list, 3);
  cl_dump(list);

  list = cl_remove(list, 6);
  cl_dump(list);

  list = cl_remove(list, 2);
  cl_dump(list);

  list = cl_remove(list, 1);
  cl_dump(list);

  cl_destroy(list);
}

void test_insert_before1(void)
{
  cl_node *list = NULL;

  printf("----- test_insert_before1 ----------------------------------------\n");
  list = cl_add_end(list, 1, "one");
  list = cl_add_end(list, 2, "two");
  list = cl_add_end(list, 3, "three");
  list = cl_add_end(list, 4, "four");
  cl_dump(list);

  list = cl_insert_before(list, 3, 101, "fred");
  cl_dump(list);

  list = cl_insert_before(list, 1, 102, "barney");
  cl_dump(list);

  list = cl_insert_before(list, 4, 103, "wilma");
  cl_dump(list);

  list = cl_insert_before(list, 102, 104, "betty");
  cl_dump(list);

  list = cl_insert_before(list, 1000, 105, "dino");
  cl_dump(list);

  cl_destroy(list);
}

void test_insert_after1(void)
{
  cl_node *list = NULL;

  printf("----- test_insert_after1 ----------------------------------------\n");
  list = cl_add_end(list, 1, "one");
  list = cl_add_end(list, 2, "two");
  list = cl_add_end(list, 3, "three");
  list = cl_add_end(list, 4, "four");
  cl_dump(list);

  cl_insert_after(list, 2, 101, "fred");
  cl_dump(list);

  cl_insert_after(list, 4, 102, "barney");
  cl_dump(list);

  cl_insert_after(list, 1, 103, "wilma");
  cl_dump(list);

  cl_insert_after(list, 102, 104, "betty");
  cl_dump(list);

  cl_insert_after(list, 1000, 105, "dino");
  cl_dump(list);

  cl_destroy(list);
}

#define VERBOSEx
void test_stress1(void)
{
  #define SIZE 4500
  cl_node *list = NULL;
  int i, count = 0;
  int *a = (int *)malloc(2 * SIZE * sizeof(int));
  char buf[LABEL_SIZE];

  printf("----- test_stress1 ----------------------------------------\n");
  for (i = 0; i < SIZE; i++)
  {
    a[count++] = i + 1;
    sprintf(buf, "%08i", a[i]);
    list = cl_add_end(list, a[i], buf); 
  }
  #ifdef VERBOSE
  cl_dump(list);
  #endif

  for (i = 0; i < SIZE; i++)
  {
    int r1 = RandomInt(0, 1);
    int r2 = RandomInt(1, count);
    a[count] = count + 1;
    count++;
    sprintf(buf, "%08i", count);

    if (r1)
      list = cl_insert_before(list, r2, count, buf);
    else
      cl_insert_after(list, r2, count, buf);

    #ifdef VERBOSE
    cl_dump(list);
    printf("%s %i", r1 ? "before" : "after", r2);
    #endif
  }
  #ifdef VERBOSE
  cl_dump(list);
  #endif

  /*PrintArray(a, count);*/
  Shuffle(a, count);
  /*PrintArray(a, count);*/

  for (i = 0; i < 2 * SIZE - 20; i++)
  {
    list = cl_remove(list, a[i]);
    #ifdef VERBOSE
    cl_dump(list);
    #endif
  }

  cl_dump(list);
  cl_destroy(list);

  free(a);
}

void test_find(int cache)
{
  cl_node *list = NULL;
  int i;
  int total = 0; 
  const int size = 10;
  int compares;

  for (i = 0; i < size; i++)
    list = cl_add_end(list, i, "node");

  cl_dump(list);

  for (i = 0; i < size; i++)
  {
    list = cl_find(list, i, cache, &compares);
    printf("Comparisons to find %i: %i\n", i, compares);
    total += compares;
  }

  list = cl_find(list, 6, cache, &compares);
  total += compares;
  list = cl_find(list, 7, cache, &compares);
  total += compares;
  list = cl_find(list, 6, cache, &compares);
  total += compares;
  list = cl_find(list, 7, cache, &compares);
  total += compares;

  for (i = 0; i < 10; i++)
  {
    list = cl_find(list, 6, cache, &compares);
    printf("Comparisons to find 6: %i\n", compares);
    total += compares;
  }

  cl_dump(list);
  printf("Total comparisons: %i\n", total);

  cl_destroy(list);
}

void test_find1(void)
{
  printf("----- test_find1 (no cache) ----------------------------------------\n");
  test_find(FALSE);
}

void test_find2(void)
{
  printf("----- test_find2 (cache) ----------------------------------------\n");
  test_find(TRUE);
}

int COUNT = 3000000;
int REPS = 100;

void test_find_stress(int cache)
{
  cl_node *list = NULL;
  int i, j;
  long total = 0;
  int *array;

  array = (int *)malloc(COUNT * sizeof(int));
  if (!array)
  {
    printf("Failed to malloc array.\n");
    abort();
  }

  for (i = 0; i < COUNT; i++)
    array[i] = i + 1;

  Shuffle(array, COUNT);

  printf("Adding... ");
  fflush(stdout);
  for (i = 0; i < COUNT; i++)
    list = cl_add_front(list, array[i], "node");
  printf("Done.\n");

  /*cl_dump(list);*/

  printf("Finding... ");
  fflush(stdout);
  for (j = 0; j < REPS; j++)
  {
    for (i = COUNT - 1; i >= COUNT - 10; i--)
    {
      int compares;
      list = cl_find(list, i, cache, &compares);
      /*cl_dump(list);*/
      /*printf("Comparisons to find %i: %i\n", i, compares);*/
      total += compares;
    }
  }
  printf("Done.\n");

  /*cl_dump(list);*/
  printf("Total comparisons: %li\n", total);

  free(array);
  cl_destroy(list);
}

void test_find_stress1(void)
{
  printf("----- test_find_stress1 (no cache) ----------------------------------------\n");
  test_find_stress(FALSE);
}

void test_find_stress2(void)
{
  printf("----- test_find_stress2 (with cache) ----------------------------------------\n");
  test_find_stress(TRUE);
}

int QUIET = 0;

void my_dump(const cl_node *list, int value1, int value2, const char *string)
{
  if (QUIET)
    return;

  printf("%s - value:%i, where:%i\n", string, value1, value2);
  cl_dump(list);
  printf("\n");
}

void test_a_lot(void)
{
  cl_node *list = NULL;
  int size = 10;
  int range = 20;
  int i;

  QUIET = 1; /* Set to 0 to see verbose output */

  printf("----- test_a_lot ----------------------------------------\n");
  for (i = 0; i < size; i++)
  {
    int value;
    int insert;

    value = RandomInt(1, range);
    list = cl_add_front(list, value, "node");
    my_dump(list, value, -1, "add_front");

    value =  RandomInt(1, range);
    list = cl_add_end(list, value, "node");
    my_dump(list, value, -1, "add_end");

    value = RandomInt(1, range);
    insert = RandomInt(1, range);
    cl_insert_after(list, insert, value, "node");
    my_dump(list, value, insert, "insert_after");

    value = RandomInt(1, range);
    insert = RandomInt(1, range);
    list = cl_insert_before(list, insert, value, "node");
    my_dump(list, value, insert, "insert_before");

    value = RandomInt(1, range);
    list = cl_remove(list, value);
    my_dump(list, value, -1, "remove");
  }
  QUIET = 0;
  cl_dump(list);

  cl_destroy(list);
}

void simple(void)
{
  cl_node *list = NULL;

  list = cl_add_end(list, 1, "");
  list = cl_add_end(list, 2, "");
  list = cl_add_end(list, 3, "");
  list = cl_add_end(list, 4, "");
  cl_dump(list);
  cl_insert_after(list, 4, 5, "");
  cl_dump(list);

  cl_destroy(list);

}

int main(int argc, char **argv)
{
  unsigned test = 0;
  unsigned count;
  void (*fp[])(void) = {
                        test_add_front,      /*  1 */
                        test_add_end,        /*  2 */
                        test_remove1,        /*  3 */
                        test_remove2,        /*  4 */
                        test_insert_before1, /*  5 */
                        test_insert_after1,  /*  6 */
                        test_find1,          /*  7 */
                        test_find2,          /*  8 */
                        test_find_stress1,   /*  9 */
                        test_find_stress2,   /* 10 */
                        test_a_lot,          /* 11 */
                       };

  count = sizeof(fp) / sizeof(*fp);

  ut_srand(1, 0);

  if (argc > 1)
    test = atoi(argv[1]);

  if (test > count)
    test = 0;

  if (test)
    fp[test - 1]();
  else
  {
    unsigned i;
    for (i = 0; i < count; i++)
      fp[i]();
  }

  return 0;
}
