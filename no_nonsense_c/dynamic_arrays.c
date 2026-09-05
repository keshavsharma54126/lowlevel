#include <stdio.h>
#include <stdlib.h>
typedef struct {
  int *items;
  size_t count;
  size_t capacity;
} Elements;

#define append(xs, i)
do {
  if (xs.count >= xs.capacity) {
    if (xs.capacity == 0)
      xs.capacity = 256;
    else
      xs.capacity *= 2;
    xs.items = realloc(xs.items, xs.capacity * sizeof(*xs.items));
  }
  xs.items[xs.count++] = i;
} while (0)

    int
    main(void) {
  Elements xs = {0};
  append(xs, 10);
  printf("hello world");
}
