#include <stdio.h>
int main() {
  printf("choose between a 1)apple, 2)pears, 3)banana\n");
  int choice;
  scanf("%d", &choice);
  switch (choice) {
  case 1:
    printf("Appples\n");
    break;
  case 2:
    printf("Pears\n");
    break;
  case 3:
    printf("banana\n");
    break;
  default:
    printf("unknown");
  }
  return 0;
}
