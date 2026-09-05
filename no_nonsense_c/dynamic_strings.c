#include <stdio.h>
#include <stdlib.h>

#define STARTING_BYTES 3;
int main() {
  unsigned int memory_size = STARTING_BYTES;
  char *text = malloc(memory_size);
  int i = 0;
  int c;
  while ((c = getc(stdin)) != EOF) {
    if (c == '\n') {
      text[i] = '\0';
      break;
    }
    // so we are doing memory_zie<= i here ? but shouldn't we basiclaly just
    // increae size when
    if (i >= memory_size) {
      memory_size = memory_size * 2;
      text = realloc(text, memory_size);
      printf("memory size increased to %d \n", memory_size);
    }
    text[i++] = c;
  }
  printf("you typed -> %s", text);
  free(text);
  return 0;
}
