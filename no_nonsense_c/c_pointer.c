#include <stdio.h>
#include <string.h>
struct Planet {
  char name[20];
  int speed;
  int diameter;
};

void init_planet(struct Planet *planet) {
  // planet->name[0] = 'E';
  // planet->name[1] = 'a';
  // planet->name[2] = 'r';
  // planet->name[2] = 't';
  // planet->name[2] = 'h';

  strcpy(planet->name, "Earth");
  planet->speed = 30;
  planet->diameter = 50000;
}

void print_planet(struct Planet *planet) {
  printf("Planet %s travels around the Sun at approximately %d km/s and has a "
         "diameter of %d km.",
         planet->name, planet->speed, planet->diameter);
}
int main() {
  printf("hello world \n");
  struct Planet planet = {};
  planet.speed = 0;
  planet.diameter = 0;
  memset(planet.name, 0, sizeof(char) * 20);
  init_planet(&planet);
  print_planet(&planet);
  return 1;
}
