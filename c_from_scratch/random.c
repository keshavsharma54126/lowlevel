#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int our_random_function(int max, bool seed) {
  int x;
  if (seed) {
    srand(getpid());
  }
  x = rand() % max + 1;
  return x;
}
int main() {
  int random;
  printf("current process'es processid = %d\n", getpid());

  random = our_random_function(10, true);
  printf("our random number = %d\n", random);
  return 0;
}
