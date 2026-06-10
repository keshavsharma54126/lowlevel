#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define AMPLITUDE 100
#define WIDTH 1000
#define HEIGHT 600
typedef struct {
  double d, angle, speed, radius, height, width;
  Color color;
} Circle;

typedef struct {
  int x, y, side;
  Color color;
} Square;
int circle_count = 1;
int square_count = 1;
void draw_square(Square *square) {
  for (int i = 0; i < square_count; i++) {
    DrawRectangle(square[i].x, square[i].y, square[i].side, square[i].side,
                  square[i].color);
  }
}
void generate_square(Square *square) {

  square[square_count - 1].x = rand() % WIDTH;
  square[square_count - 1].y = rand() % HEIGHT;
  square[square_count - 1].side = rand() % 100;
  // now lets also genrate the circle color
  square[square_count - 1].color.r = rand() % 256;

  square[square_count - 1].color.g = rand() % 256;

  square[square_count - 1].color.b = rand() % 256;
  square[square_count - 1].color.a = 255;
}
void draw_circle(Circle *circle) {
  for (int i = 0; i < circle_count; i++) {
    double x = circle[i].d * sin(circle[i].angle) + circle[i].width / 2;
    double y = circle[i].d * cos(circle[i].angle) + circle[i].height / 2;
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
  circle[circle_count - 1].height = HEIGHT;
  circle[circle_count - 1].width = WIDTH;
}
void generate_circle_on_point(Circle *circle, double height, double width) {

  circle[circle_count - 1].d = rand() % 100;
  circle[circle_count - 1].angle = 0;
  circle[circle_count - 1].speed = rand() % 9 + 1;
  circle[circle_count - 1].radius = rand() % 50;
  circle[circle_count - 1].height = height;
  circle[circle_count - 1].width = width;
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
  Square *square_memory = malloc(sizeof(Square));
  generate_square(&square_memory[square_count - 1]);
  InitWindow(WIDTH, HEIGHT, "my first animation");
  SetTargetFPS(120);
  while (!WindowShouldClose()) {
    KeyboardKey key = KEY_ENTER;
    if (IsKeyPressed(key)) {
      printf("key pressed \n");
      circle_count++;
      circle_memory = realloc(circle_memory, circle_count * sizeof(Circle));
      Circle *new_circle = circle_memory;
      generate_circle(new_circle);
    }
    if (IsKeyPressed(KEY_SPACE)) {
      printf("key pressed \n");
      square_count++;
      square_memory = realloc(square_memory, square_count * sizeof(Square));
      Square *new_square_memory = square_memory;
      generate_square(new_square_memory);
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      printf("Mouse button pressed \n");
      Vector2 mousePos = GetMousePosition();
      circle_count++;
      circle_memory = realloc(circle_memory, circle_count * sizeof(Circle));
      Circle *new_circle = circle_memory;
      generate_circle_on_point(new_circle, 2 * mousePos.y, 2 * mousePos.x);
    }
    move_circle(circle_memory, GetFrameTime());
    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(20, 20);
    DrawText("ok this is a cirle moving in a circle!", 120, 20, 20, LIGHTGRAY);
    draw_circle(circle_memory);
    draw_square(square_memory);
    EndDrawing();
  }

  CloseWindow();
  free(circle_memory);
  free(square_memory);

  return 0;
}
