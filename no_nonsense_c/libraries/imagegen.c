#include "TinyPngOut.h"
#include <stdint.h>
#include <stdio.h>
struct Person {
  char *name;
  int age;
};

void printPerson(struct Person p) {
  printf("person's name is %s and his age is %d \n", p.name, p.age);
}
void printPersonPointer(struct Person *p) {
  printf("person's name is %s and his age is %d \n", p->name, p->age);
}

int main(int argc, char *argv[]) {
  printf("generating image.... \n");
  struct Person person = {.name = "Keshav", .age = 25};
  printPerson(person);
  printPersonPointer(&person);

  // now we will do some imagegen stuff
  uint32_t width = 400;
  uint32_t height = 400;
  FILE *outputfile = fopen("image.png", "w");
  struct TinyPngOut tp;
  enum TinyPngOut_Status status =
      TinyPngOut_init(&tp, width, height, outputfile);

  if (status != TINYPNGOUT_OK) {
    return -1;
  }

  uint8_t saffron[] = {255, 153, 51};
  uint8_t white[] = {255, 255, 255};
  uint8_t green[] = {19, 136, 8};
  for (int i = 0; i < height; i++) {

    for (int j = 0; j < width; j++) {
      if (i < height / 3) {
        enum TinyPngOut_Status write_status = TinyPngOut_write(&tp, saffron, 1);
      } else if (i < 2 * height / 3) {
        enum TinyPngOut_Status write_status = TinyPngOut_write(&tp, white, 1);
      } else {
        enum TinyPngOut_Status write_status = TinyPngOut_write(&tp, green, 1);
      }
    }
  }
  fclose(outputfile);
  printf("Done \n");
  return 1;
}
