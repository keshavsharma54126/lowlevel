#include <stdio.h>

#define ENCRYPTOIN_KEY 69
int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("%s <infile> ", argv[0]);
    return -1;
  }

  FILE *input_file = fopen(argv[1], "r");

  int c;
  int linebreak_counter = 0;
  while ((c = getc(input_file)) != EOF) {
    if (c == 10) {
      linebreak_counter += 1;
    }
  }

  fclose(input_file);
  printf("linebreak_counter = %d \n", linebreak_counter);
  return 0;
}
