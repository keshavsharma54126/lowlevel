#include <stdio.h>

int main() {
  for (int i = 0; i < 20; i++) {
    putchar(i % 3 ? '*' : '\n');
  }
}
