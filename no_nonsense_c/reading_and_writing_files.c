#include <stdio.h>

#define ENCRYPTOIN_KEY 69
int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("%s <infile> <outfile>", argv[0]);
    return -1;
  }

  FILE *input_file = fopen(argv[1], "r");
  FILE *output_file = fopen(argv[2], "w");

  int c;
  while ((c = getc(input_file)) != EOF) {
    int encryption_salt = 0;
    if (argv[3] == "1") {
      encryption_salt = ENCRYPTOIN_KEY;
    }
    putc(c ^ ENCRYPTOIN_KEY, output_file);
  }

  fclose(input_file);
  fclose(output_file);

  return 0;
}
