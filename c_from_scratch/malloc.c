#include <stdio.h>
#include <stdlib.h>
int main() {
  char *name;

  name = malloc(32);
  printf("please enter your name \n");
  fgets(name, 32, stdin);
  printf("your name is %s\n", name);
  free(name);
  return 0;
}
