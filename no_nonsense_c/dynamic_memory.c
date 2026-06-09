#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define AMPLITUDE 100
#define WIDTH 1000
#define HEIGHT 600
typedef struct {
  double d, angle, speed, radius;
  Color color;
} Circle;

int circle_count = 1;
void draw_circle(Circle *circle) {
  for (int i = 0; i < circle_count; i++) {
    double x = circle[i].d * sin(circle[i].angle) + WIDTH / 2;
    double y = circle[i].d * cos(circle[i].angle) + HEIGHT / 2;
    DrawCircle(x, y, circle[i].radius, circle[i].color);
  }
}

void move_circle(Circle *circle, float time_difference) {
  for (int i = 0; i < circle_count; i++) {
    circle[i].angle += circle->speed * time_difference;
  }
}

void generate_circle(Circle *circle) {

  circle[circle_count - 1].d = rand() % 100;
  circle[circle_count - 1].angle = 0;
  circle[circle_count - 1].speed = rand() % 9 + 1;
  circle[circle_count - 1].radius = rand() % 50;

  // now lets also genrate the circle color
  circle[circle_count - 1].color.r = rand() % 256;

  circle[circle_count - 1].color.g = rand() % 256;

  circle[circle_count - 1].color.b = rand() % 256;
  circle[circle_count - 1].color.a = 255;
}
int main(void) {
  srand(time(NULL));
  Circle *circle_memory = malloc(sizeof(Circle));
  generate_circle(&circle_memory[circle_count - 1]);
  InitWindow(WIDTH, HEIGHT, "my first animation");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    KeyboardKey key = KEY_ENTER;
    if (IsKeyPressed(key)) {
      printf("key pressed \n");
      circle_count++;
      circle_memory = realloc(circle_memory, circle_count * sizeof(Circle));
      Circle *new_circle = circle_memory;
      generate_circle(new_circle);
    }
    move_circle(circle_memory, GetFrameTime());
    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(20, 20);
    DrawText("ok this is a cirle moving in a circle!", 120, 20, 20, LIGHTGRAY);
    draw_circle(circle_memory);
    EndDrawing();
  }

  CloseWindow();
  free(circle_memory);

  return 0;
}
