#include <stdio.h>

void addNumbers(int *target, int x, int y) {
  *target = x + y;
  return;
}
int main() {
  int x, y, target;
  x = 10;
  y = 90;

  addNumbers(&target, x, y);
  printf("%d\n", target);
  return 0;
}
