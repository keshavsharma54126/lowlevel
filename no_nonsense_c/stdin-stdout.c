#include <ctype.h>
#include <stdio.h>
#define ENCRYPTION_KEY 42

int main() {
  char c;
  while ((c = getc(stdin)) != EOF) {

    putc(c ^ ENCRYPTION_KEY, stdout);
  }
  return 0;
}
