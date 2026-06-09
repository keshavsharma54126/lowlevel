#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
  int sum = 0;
  for (int i = 0; i < argc; i++) {
    int number = atoi(argv[i]);
    sum = sum + number;
  }
  printf("the sum is %d,", sum);
}
