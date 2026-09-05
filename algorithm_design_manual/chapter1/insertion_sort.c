// we are starting with sorting rightt and here this is a commmon sorting
// algoritym also known as insertion srot

#include <stdio.h>
#include <stdlib.h>

void printfArray(int *arr, int count) {
  for (int i = 0; i < count; i++) {
    printf("%d element = %d\n", i, arr[i]);
  }
  printf("\n");
}
int main() {
  printf("hello world\n");
  int i, j;
  printf("enter the no. of digits you want to sort\n");
  int c;
  scanf("%d", &c);

  int *arr = malloc(c * sizeof(int));

  printf("enter the numbers that you wnat to sort \n");
  int counter = 0;
  int count = c;
  while (count > 0) {
    int num;
    scanf("%d", &num);
    arr[counter] = num;
    counter++;
    count--;
  }
  printfArray(arr, c);
  for (int i = 0; i < c; i++) {
    j = i;
    while (j < c) {
      if (arr[j] < arr[i]) {
        int temp = arr[j];
        arr[j] = arr[i];
        arr[i] = temp;
      }
      j++;
    }
  }
  printfArray(arr, c);
  free(arr);
  return 0;
}
