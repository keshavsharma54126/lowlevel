#include <stdio.h>
#include <string.h>
#define COUNT 256

void printFrequencyAsciiBars(int *arr, int count) {
  for (int i = 0; i < count; i++) {
    char ascii_string[100] = "";

    if (arr[i] == 0) {
      continue;
    } else {
      int pos = 0;
      while (arr[i] > 0) {
        ascii_string[pos++] += '#';
        // or we can use strcat(ascii_string,"#")
        arr[i]--;
      }
      printf("\n%c | %s\n", i, ascii_string);
    }
  };
}
int main() {

  int arr[256];

  memset(arr, 0, COUNT * sizeof(int));
  int c;
  while ((c = getc(stdin)) != EOF) {
    arr[c] += 1;
  }

  printFrequencyAsciiBars(arr, COUNT);
  return 0;
}
