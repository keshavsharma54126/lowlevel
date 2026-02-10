#include <stdio.h>
#include <unistd.h>

int main() {
  printf("countdown starting \n");
  fflush(stdout);
  int countdown = 10;
  while (1) {
    if (countdown == 3) {
      countdown -= 1;
      continue;
    }
    printf("tick tick %d\n", countdown);
    fflush(stdout);
    sleep(1);
    countdown -= 1;
    if (countdown == -1) {
      break;
    }
  }
  return 0;
}
