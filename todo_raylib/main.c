#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WIDTH 1000
#define HEIGHT 1000

int FontSize = 18;
int ContentFontSize = 18;
// todo settings struct might add things to it later
typedef struct Settigns {
  bool settingsOpen;
  float fontSize;
  Color bgColor;
  float opacity;
  bool darkMode;
  bool showCompleted;
  int themeIndex;
  bool themeEditMode;
  Font customFont;
} Settings;

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

  SetConfigFlags(FLAG_WINDOW_TRANSPARENT);

  Font customFont = LoadFont("./fonts/monospace/MonoSpace-Bold.ttf");
  const Settings defaultConfig = {false, 18, BLACK, 1,         true,
                                  true,  0,  false, customFont};
  Settings setting = defaultConfig;
  setting.customFont = customFont;
  InitWindow(WIDTH, HEIGHT, "Todo List ");
  SetTargetFPS(60);
  GuiSetFont(customFont);
  bool settingsOpen = false;

  SetExitKey(KEY_NULL);

  bool exitWindow = false;
  bool showExitPrompt = false;
  while (!exitWindow) {
    if (WindowShouldClose()) {
      showExitPrompt = true;
    }
    BeginDrawing();
    ClearBackground(Fade(setting.bgColor, setting.opacity));

    Rectangle screenRect = {0, 0, (float)GetScreenWidth(),
                            (float)GetScreenHeight()};
    DrawRectangleLinesEx(screenRect, 0.2f, RAYWHITE);
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
    int btnActive = 0;
    bool secretViewActive = true;
    char *todoText = "";
    GuiSetStyle(DEFAULT, TEXT_SIZE, setting.fontSize);
    GuiTextInputBox((Rectangle){WIDTH / 4.00, 50, WIDTH / 2.00, 100}, "Todos",
                    "", todoText, 10, "create", &btnActive, &secretViewActive);

    GuiSetStyle(DEFAULT, TEXT_SIZE, FontSize);
    if (GuiButton((Rectangle){WIDTH - 140, 10, 120, 40}, "#142# Settings"))
      settingsOpen = !settingsOpen;

    if (settingsOpen) {
      Rectangle win = {WIDTH - 420, 60, 400, 360};
      float x = win.x + 20;
      float w = win.width - 40;
      float y = win.y + 40;

      if (setting.themeEditMode)
        GuiLock();

      if (GuiWindowBox(win, "#142# Settings"))
        settingsOpen = false;

      // --- Appearance section ---
      GuiGroupBox((Rectangle){x, y, w, 150}, "Appearance");
      y += 15;
      GuiSlider((Rectangle){x + 90, y, w - 150, 20}, "Font size",
                TextFormat("%d", (int)setting.fontSize), &setting.fontSize, 12,
                32);
      y += 35;
      GuiSlider((Rectangle){x + 90, y, w - 150, 20}, "Opacity",
                TextFormat("%0.2f", (float)setting.opacity), &setting.opacity,
                0.30, 1.00);
      y += 35;
      // dropdown LAST so its open list draws on top of everything above
      GuiLabel((Rectangle){x + 30, y, w - 150, 20}, "Theme");

      if (GuiDropdownBox((Rectangle){x + 90, y, w - 150, 20},
                         "Default;Dark;Cyber;Jungle", &setting.themeIndex,
                         setting.themeEditMode))
        setting.themeEditMode = !setting.themeEditMode;

      y += 35;
      GuiCheckBox((Rectangle){x + 10, y, 20, 20}, "Dark mode",
                  &setting.darkMode);

      // --- Behaviour section ---
      y += 50;
      GuiGroupBox((Rectangle){x, y, w, 60}, "Behaviour");
      GuiCheckBox((Rectangle){x + 10, y + 20, 20, 20}, "Show completed todos",
                  &setting.showCompleted);

      GuiUnlock();
    }
    EndDrawing();
  }
  printf("exited alrady");
  UnloadFont(customFont);
  CloseWindow();
  return 0;
}
