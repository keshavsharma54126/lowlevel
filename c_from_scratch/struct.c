#include <stdio.h>
#include <string.h>

struct person {
  char firstName[8];
  char lastName[32];
  int age;
};

int main() {
  struct person keshav;

  strcpy(keshav.firstName, "Keshav");
  strcpy(keshav.lastName, "Sharma");
  keshav.age = 26;
  printf("titlle = %s\n lastName = %s\n age = %d\n", keshav.firstName, keshav.lastName,
         keshav.age);
  return 0;
}
