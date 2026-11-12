#include <stdio.h>
#include <string.h>

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WIDTH 1000
#define HEIGHT 1000

#define MAX_TODOS 256
#define TODO_LEN 128
#define SAVE_FILE "todos.txt"

#define COL_W 640.0f
#define COL_X ((WIDTH - COL_W) / 2.0f)
#define LIST_TOP 290.0f
#define LIST_BOTTOM 900.0f

// ---------------------------------------------------------------- themes
typedef struct {
  Color bg, surface, hover, border, text, muted, accent, danger;
} Palette;

static const Palette THEMES[] = {
    // Default: slate + indigo
    {{15, 23, 42, 255},
     {30, 41, 59, 255},
     {51, 65, 85, 255},
     {71, 85, 105, 255},
     {241, 245, 249, 255},
     {148, 163, 184, 255},
     {129, 140, 248, 255},
     {244, 63, 94, 255}},
    // Dark: neutral greys
    {{10, 10, 10, 255},
     {23, 23, 23, 255},
     {38, 38, 38, 255},
     {64, 64, 64, 255},
     {250, 250, 250, 255},
     {163, 163, 163, 255},
     {229, 229, 229, 255},
     {239, 68, 68, 255}},
    // Cyber: purple + cyan
    {{13, 2, 33, 255},
     {30, 10, 60, 255},
     {50, 20, 90, 255},
     {90, 40, 140, 255},
     {240, 230, 255, 255},
     {170, 140, 210, 255},
     {0, 229, 255, 255},
     {255, 0, 110, 255}},
    // Jungle: deep green + lime
    {{6, 24, 18, 255},
     {16, 44, 33, 255},
     {24, 64, 48, 255},
     {40, 90, 68, 255},
     {230, 255, 240, 255},
     {140, 190, 165, 255},
     {163, 230, 53, 255},
     {248, 113, 113, 255}},
};

// ---------------------------------------------------------------- data
typedef struct {
  char text[TODO_LEN];
  bool done;
} Todo;

typedef struct Settings {
  bool open;
  float fontSize;
  float opacity;
  bool showCompleted;
  bool showProgress;
  int themeIndex;
  bool themeEditMode;
} Settings;

static const Settings DEFAULT_SETTINGS = {
    .open = false,
    .fontSize = 20,
    .opacity = 1.0f,
    .showCompleted = true,
    .showProgress = true,
    .themeIndex = 0,
    .themeEditMode = false,
};

static Todo todos[MAX_TODOS];
static int todoCount = 0;

// ---------------------------------------------------------------- persistence
static void SaveTodos(void) {
  FILE *f = fopen(SAVE_FILE, "w");
  if (!f)
    return;
  for (int i = 0; i < todoCount; i++)
    fprintf(f, "%d %s\n", todos[i].done ? 1 : 0, todos[i].text);
  fclose(f);
}

static void LoadTodos(void) {
  FILE *f = fopen(SAVE_FILE, "r");
  if (!f)
    return;
  char line[TODO_LEN + 4];
  while (todoCount < MAX_TODOS && fgets(line, sizeof line, f)) {
    line[strcspn(line, "\r\n")] = '\0';
    if (strlen(line) < 3)
      continue;
    todos[todoCount].done = (line[0] == '1');
    snprintf(todos[todoCount].text, TODO_LEN, "%s", line + 2);
    todoCount++;
  }
  fclose(f);
}

// ---------------------------------------------------------------- helpers
static void ApplyTheme(Palette p) {
  // DEFAULT base props propagate to every control
  GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(p.border));
  GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(p.surface));
  GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(p.text));
  GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(p.accent));
  GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(p.hover));
  GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(p.text));
  GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, ColorToInt(p.accent));
  GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(p.accent));
  GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(p.bg));
  GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED, ColorToInt(p.border));
  GuiSetStyle(DEFAULT, BASE_COLOR_DISABLED, ColorToInt(p.surface));
  GuiSetStyle(DEFAULT, TEXT_COLOR_DISABLED, ColorToInt(p.muted));
  GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(p.surface));
  GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(p.border));
  GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
  GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
  GuiSetStyle(DEFAULT, BORDER_WIDTH, 1);

  // buttons = accent filled
  GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(p.accent));
  GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(p.accent));
  GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(p.bg));
  GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED,
              ColorToInt(ColorBrightness(p.accent, 0.15f)));
  GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(p.bg));
  GuiSetStyle(BUTTON, BASE_COLOR_PRESSED,
              ColorToInt(ColorBrightness(p.accent, -0.15f)));

  GuiSetStyle(TEXTBOX, TEXT_PADDING, 12);
}

static bool IsVisible(const Todo *t, int filter, bool showCompleted) {
  if (filter == 1)
    return !t->done; // Active
  if (filter == 2)
    return t->done;                 // Done
  return showCompleted || !t->done; // All
}

// cut text with "..." so it never runs under the delete button
static const char *Ellipsize(Font f, const char *txt, float fs, float maxW) {
  static char buf[TODO_LEN + 4];
  if (MeasureTextEx(f, txt, fs, 1).x <= maxW)
    return txt;
  int n = (int)strlen(txt);
  while (n > 0) {
    snprintf(buf, sizeof buf, "%.*s...", n, txt);
    if (MeasureTextEx(f, buf, fs, 1).x <= maxW)
      break;
    n--;
  }
  return buf;
}

static void DrawTick(Vector2 c, bool done, bool hot, Palette p) {
  if (done) {
    DrawCircleV(c, 12, p.accent);
    DrawLineEx((Vector2){c.x - 5, c.y}, (Vector2){c.x - 1, c.y + 4}, 2.5f,
               p.bg);
    DrawLineEx((Vector2){c.x - 1, c.y + 4}, (Vector2){c.x + 6, c.y - 4}, 2.5f,
               p.bg);
  } else {
    DrawRing(c, 10, 12, 0, 360, 36, hot ? p.accent : p.muted);
  }
}

static void DrawSettings(Settings *s, Rectangle win, bool locked) {
  float x = win.x + 16, w = win.width - 32, y = win.y + 44;

  if (locked || s->themeEditMode)
    GuiLock();

  if (GuiWindowBox(win, "#142# Settings")) {
    s->open = false;
    s->themeEditMode = false;
  }

  // --- Appearance ---
  GuiGroupBox((Rectangle){x, y, w, 140}, "Appearance");
  float themeY = y + 16; // dropdown is drawn last, remember its row
  GuiSlider((Rectangle){x + 120, y + 60, w - 190, 20}, "Font size",
            TextFormat("%d", (int)s->fontSize), &s->fontSize, 14, 32);
  GuiSlider((Rectangle){x + 120, y + 100, w - 190, 20}, "Opacity",
            TextFormat("%d%%", (int)(s->opacity * 100)), &s->opacity, 0.3f,
            1.0f);

  // --- Behaviour ---
  y += 160;
  GuiGroupBox((Rectangle){x, y, w, 90}, "Behaviour");
  GuiCheckBox((Rectangle){x + 14, y + 18, 20, 20}, "Show completed in All",
              &s->showCompleted);
  GuiCheckBox((Rectangle){x + 14, y + 52, 20, 20}, "Show progress bar",
              &s->showProgress);

  y += 110;
  if (GuiButton((Rectangle){x, y, w, 36}, "Reset to defaults")) {
    *s = DEFAULT_SETTINGS;
    s->open = true;
  }

  if (!locked)
    GuiUnlock();

  // dropdown LAST so its open list draws over everything
  GuiLabel((Rectangle){x + 14, themeY, 100, 30}, "Theme");
  if (GuiDropdownBox((Rectangle){x + 120, themeY, w - 134, 30},
                     "Default;Dark;Cyber;Jungle", &s->themeIndex,
                     s->themeEditMode))
    s->themeEditMode = !s->themeEditMode;

  GuiUnlock();
}

// ---------------------------------------------------------------- main
int main(void) {
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_MSAA_4X_HINT);
  InitWindow(WIDTH, HEIGHT, "Todos"); // must come BEFORE LoadFont
  SetTargetFPS(60);
  SetExitKey(KEY_NULL);

  Font font = LoadFontEx("./fonts/monospace/SpaceMono-Bold.ttf", 64, NULL, 0);
  SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
  GuiSetFont(font);

  Settings s = DEFAULT_SETTINGS;
  LoadTodos();

  char input[TODO_LEN] = ""; // real writable buffer, not a string literal
  bool inputEdit = true;
  int filter = 0;
  float scroll = 0;

  bool showExitPrompt = false;
  bool exitWindow = false;

  while (!exitWindow) {
    if (WindowShouldClose())
      showExitPrompt = true;

    Palette p = THEMES[s.themeIndex];
    ApplyTheme(p);

    Vector2 mouse = GetMousePosition();
    Rectangle settingsRect = {WIDTH - 420, 60, 400, 370};
    bool overSettings = s.open && CheckCollisionPointRec(mouse, settingsRect);
    bool blockMain = showExitPrompt || overSettings || s.themeEditMode;

    int doneCount = 0;
    for (int i = 0; i < todoCount; i++)
      if (todos[i].done)
        doneCount++;
    int left = todoCount - doneCount;

    BeginDrawing();
    ClearBackground(Fade(p.bg, s.opacity));

    if (blockMain)
      GuiLock();

    // ---------- header
    DrawTextEx(font, "My Tasks", (Vector2){COL_X, 40}, 44, 1, p.text);
    DrawTextEx(font, TextFormat("%d of %d completed", doneCount, todoCount),
               (Vector2){COL_X, 96}, 18, 1, p.muted);

    if (s.showProgress) {
      Rectangle bar = {COL_X, 130, COL_W, 6};
      DrawRectangleRounded(bar, 1, 8, p.surface);
      if (todoCount > 0 && doneCount > 0) {
        bar.width = COL_W * doneCount / (float)todoCount;
        DrawRectangleRounded(bar, 1, 8, p.accent);
      }
    }

    if (GuiButton((Rectangle){WIDTH - 150, 10, 130, 40}, "#142# Settings")) {
      s.open = !s.open;
      s.themeEditMode = false;
    }

    // ---------- input row
    Rectangle inputRect = {COL_X, 160, COL_W - 130, 50};
    bool submit = false;

    GuiSetStyle(DEFAULT, TEXT_SIZE, (int)s.fontSize);
    if (GuiTextBox(inputRect, input, TODO_LEN - 1, inputEdit)) {
      if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
        submit = true;
      else
        inputEdit = !inputEdit;
    }
    if (input[0] == '\0') {
      DrawTextEx(font, "What needs to be done?",
                 (Vector2){inputRect.x + 14,
                           inputRect.y + (inputRect.height - s.fontSize) / 2},
                 s.fontSize, 1, p.muted);
    }
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);

    if (GuiButton((Rectangle){COL_X + COL_W - 120, 160, 120, 50}, "+ Add"))
      submit = true;

    if (submit) {
      char *t = input;
      while (*t == ' ')
        t++;
      size_t n = strlen(t);
      while (n > 0 && t[n - 1] == ' ')
        t[--n] = '\0';

      if (n > 0 && todoCount < MAX_TODOS) {
        // newest on top
        memmove(&todos[1], &todos[0], todoCount * sizeof(Todo));
        snprintf(todos[0].text, TODO_LEN, "%s", t);
        todos[0].done = false;
        todoCount++;
        scroll = 0;
        SaveTodos();
      }
      input[0] = '\0';
      inputEdit = true;
    }

    // ---------- filter row
    GuiToggleGroup((Rectangle){COL_X, 230, 100, 36}, "All;Active;Done",
                   &filter);

    if (doneCount == 0)
      GuiDisable();
    if (GuiButton((Rectangle){COL_X + COL_W - 190, 230, 190, 36},
                  "Clear completed")) {
      int j = 0;
      for (int i = 0; i < todoCount; i++)
        if (!todos[i].done)
          todos[j++] = todos[i];
      todoCount = j;
      SaveTodos();
    }
    GuiEnable();

    // ---------- list
    float itemH = s.fontSize + 36;
    float gap = 10;
    Rectangle listArea = {COL_X - 10, LIST_TOP, COL_W + 20,
                          LIST_BOTTOM - LIST_TOP};

    int visible = 0;
    for (int i = 0; i < todoCount; i++)
      if (IsVisible(&todos[i], filter, s.showCompleted))
        visible++;

    bool mouseInList = !blockMain && CheckCollisionPointRec(mouse, listArea);
    if (mouseInList)
      scroll += GetMouseWheelMove() * 40;

    float contentH = visible * (itemH + gap);
    float minScroll = listArea.height - contentH;
    if (minScroll > 0)
      minScroll = 0;
    if (scroll < minScroll)
      scroll = minScroll;
    if (scroll > 0)
      scroll = 0;

    int toDelete = -1;
    BeginScissorMode((int)listArea.x, (int)listArea.y, (int)listArea.width,
                     (int)listArea.height);

    float y = LIST_TOP + scroll;
    for (int i = 0; i < todoCount; i++) {
      if (!IsVisible(&todos[i], filter, s.showCompleted))
        continue;

      Rectangle r = {COL_X, y, COL_W, itemH};
      y += itemH + gap;
      if (r.y + r.height < LIST_TOP || r.y > LIST_BOTTOM)
        continue; // off-screen

      bool hot = mouseInList && CheckCollisionPointRec(mouse, r);
      Vector2 xc = {r.x + r.width - 32, r.y + itemH / 2};
      bool hotX = hot && CheckCollisionPointCircle(mouse, xc, 14);

      DrawRectangleRounded(r, 0.25f, 8, hot ? p.hover : p.surface);
      DrawTick((Vector2){r.x + 32, r.y + itemH / 2}, todos[i].done,
               hot && !hotX, p);

      const char *label =
          Ellipsize(font, todos[i].text, s.fontSize, r.width - 130);
      Vector2 ts = MeasureTextEx(font, label, s.fontSize, 1);
      Vector2 tp = {r.x + 60, r.y + (itemH - ts.y) / 2};
      DrawTextEx(font, label, tp, s.fontSize, 1,
                 todos[i].done ? p.muted : p.text);
      if (todos[i].done)
        DrawLineEx((Vector2){tp.x, tp.y + ts.y / 2},
                   (Vector2){tp.x + ts.x, tp.y + ts.y / 2}, 2, p.muted);

      if (hot) { // delete "x"
        Color xcol = hotX ? p.danger : p.muted;
        DrawLineEx((Vector2){xc.x - 6, xc.y - 6}, (Vector2){xc.x + 6, xc.y + 6},
                   2, xcol);
        DrawLineEx((Vector2){xc.x + 6, xc.y - 6}, (Vector2){xc.x - 6, xc.y + 6},
                   2, xcol);
      }

      if (hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (hotX)
          toDelete = i;
        else {
          todos[i].done = !todos[i].done;
          SaveTodos();
        }
      }
    }
    EndScissorMode();

    if (toDelete >= 0) {
      memmove(&todos[toDelete], &todos[toDelete + 1],
              (todoCount - toDelete - 1) * sizeof(Todo));
      todoCount--;
      SaveTodos();
    }

    if (visible == 0) {
      const char *msg = todoCount == 0 ? "No tasks yet. Add one above!"
                                       : "Nothing in this view.";
      Vector2 ms = MeasureTextEx(font, msg, 20, 1);
      DrawTextEx(font, msg,
                 (Vector2){COL_X + (COL_W - ms.x) / 2, LIST_TOP + 60}, 20, 1,
                 p.muted);
    }

    // ---------- footer
    DrawTextEx(font, TextFormat("%d item%s left", left, left == 1 ? "" : "s"),
               (Vector2){COL_X, 925}, 18, 1, p.muted);
    const char *hint = "Enter to add  |  click to complete";
    Vector2 hs = MeasureTextEx(font, hint, 16, 1);
    DrawTextEx(font, hint, (Vector2){COL_X + COL_W - hs.x, 927}, 16, 1,
               p.muted);

    GuiUnlock();

    // ---------- settings window (above the list)
    if (s.open)
      DrawSettings(&s, settingsRect, showExitPrompt);

    // ---------- exit prompt (above everything)
    if (showExitPrompt) {
      DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.55f));
      int btn = -1;
      GuiMessageBox(
          (Rectangle){WIDTH / 2.0f - 150, HEIGHT / 2.0f - 70, 300, 140},
          "#191#Quit", "Close the app?", "Quit;Cancel", &btn);
      if (btn == 1)
        exitWindow = true; // Quit
      else if (btn == 0 || btn == 2)
        showExitPrompt = false; // X or Cancel
    }

    EndDrawing();
  }

  SaveTodos();
  UnloadFont(font);
  CloseWindow();
  return 0;
}
