#include "raylib.h"
#include <math.h>

#define AMPLITUDE 100
int main(void) {
  int width = 800;
  int height = 450;
  InitWindow(width, height, "my first animation");
  double angle = 0;
  double x, y;
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(20, 20);
    DrawText("ok this is a cirle moving in a circle!", 120, 20, 20, LIGHTGRAY);
    Color mycolor = {255, 0, 0, 255};
    Color secondColor = {0, 255, 0, 255};
    Color thirdColor = {0, 0, 255, 255};
    x = AMPLITUDE * sin(angle) + width / 2;
    y = AMPLITUDE * cos(angle) + height / 2;

    DrawCircle(x, y, 51.00, mycolor);
    x = AMPLITUDE * 0.5 * sin(angle - 3) + width / 2;
    y = AMPLITUDE * 0.5 * cos(angle - 3) + height / 2;

    DrawCircle(x, y, 20.0 0, secondColor);
    x = AMPLITUDE * 0.2 * sin(angle - 2) + width / 2;

    y = AMPLITUDE * 0.2 * cos(angle - 2) + height / 2;

    DrawCircle(x, y, 12.00, thirdColor);

    angle += 0.04;

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
