#include <assert.h>
#include <stdio.h>

#define F fflush(stdout)

void multi(int table, int i) {
  int result;
  result = table * i;
  printf("%d x %d = %d\n", table, i, result);
  return;
}
int main() {
  int x, table;
  printf("select a multiplicatoin table\n");
  F;
  scanf("%d", &table);
  assert((table < 13) && (table > 0));
  for (int i = 0; i < 13; i++) {
    multi(table, i);
  }
  return 0;
}
