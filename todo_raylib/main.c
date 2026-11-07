#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// typedef struct Color {
//   unsigned char r; // Color red value
//   unsigned char g; // Color green value
//   unsigned char b; // Color blue value
//   unsigned char a; // Color alpha value
// } Color;
//
#define WIDTH 1000
#define HEIGHT 1000

int FontSize = 18;

void SetDefaultStyles() {
  GuiSetStyle(DEFAULT, TEXT_SIZE, FontSize);
  GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(DARKBLUE));
  GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(BLUE));
  GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(SKYBLUE));

  // Text/Icon Colors
  GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
  GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(YELLOW));
  GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(WHITE));

  // Border Colors
  GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(BLACK));
}
int main() {
  InitWindow(WIDTH, HEIGHT, "Todo List ");
  SetTargetFPS(60);
  Font customFont = LoadFont("./fonts/monospace/SpaceMono-Bold.ttf");
  GuiSetFont(customFont);
  Color BackgroundColor = {.r = 32, .g = 32, .b = 32, .a = 100};
  bool showColorPallete = false;

  SetExitKey(KEY_NULL);

  bool exitWindow = false;
  bool showExitPrompt = false;
  bool clickedYes = false;
  while (!exitWindow) {
    if (WindowShouldClose()) {
      showExitPrompt = true;
    }
    BeginDrawing();
    ClearBackground(BackgroundColor);
    SetDefaultStyles();

    // show exit promtp and confirm if
    // showExitPrompt is true
    if (showExitPrompt) {
      int btnActive = -1;

      GuiMessageBox((Rectangle){WIDTH / 2.0 - 125, HEIGHT / 2.0 - 50, 250, 100},
                    "#191#Close Window", "Do you wish to exit?", "Yes;No",
                    &btnActive);

      if (btnActive == 0 || btnActive == 2) {
        printf("button pressed %d", btnActive);
        exitWindow = false;
        showExitPrompt = false;
      }

      else if (btnActive == 1) {
        printf("button pressed %d", btnActive);
        exitWindow = true;
        showExitPrompt = false;
      }
    }

    if (GuiButton((Rectangle){WIDTH - 140, 10, 120, 40}, "#191# Settings"))
      showColorPallete = !showColorPallete;

    if (showColorPallete) {
      int btnActive = -1;

      GuiMessageBox((Rectangle){WIDTH - 260, 50, 250, 100}, "#191#Message Box",
                    "Hi! This is a message!", "Nice;Cool;Ok", &btnActive);

      if (btnActive >= 0)
        showColorPallete = false;
    }

    EndDrawing();
  }
  printf("exited alrady");
  UnloadFont(customFont);
  CloseWindow();
  return 0;
}
