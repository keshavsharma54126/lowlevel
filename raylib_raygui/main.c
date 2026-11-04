#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main() {
  InitWindow(800, 800, "raygui - controls test suite");
  SetTargetFPS(60);

  int btnActive = -1;
  bool showMessageBox = false;
  // background image/ texture
  Texture2D background = LoadTexture("raylib_raygui/images.jpeg");
  while (!WindowShouldClose()) {
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(WHITE);
    DrawTexture(background, 0, 0, WHITE);
    if (GuiButton((Rectangle){24, 24, 120, 30}, "#191#Show Message"))
      showMessageBox = true;

    if (showMessageBox) {
      GuiMessageBox((Rectangle){85, 70, 250, 100}, "#191#Message Box",
                    "Hi! This is a message!", "Nice;Cool", &btnActive);

      if (btnActive >= 0)
        showMessageBox = false;
    }

    EndDrawing();
  }
  UnloadTexture(background);
  CloseWindow();
  return 0;
}
