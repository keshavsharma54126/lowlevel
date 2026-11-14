// Demo for ui.h: a small todo screen. Resize the window to watch the layout adapt.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "ui.h"

#define C_BG ((Color){15, 23, 42, 255})
#define C_SURFACE ((Color){30, 41, 59, 255})
#define C_HOVER ((Color){51, 65, 85, 255})
#define C_TEXT ((Color){241, 245, 249, 255})
#define C_MUTED ((Color){148, 163, 184, 255})
#define C_ACCENT ((Color){99, 102, 241, 255})
#define C_DANGER ((Color){225, 29, 72, 255})

#define MAX_TODOS 32

typedef struct {
  char text[64];
  bool done;
} Todo;

int main(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
  InitWindow(900, 600, "ui.h demo");
  SetTargetFPS(60);

  Font font = LoadFontEx("../todo_raylib/fonts/monospace/SpaceMono-Bold.ttf", 48, NULL, 0);
  if (font.texture.id != GetFontDefault().texture.id) {
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    UI_SetFont(font);
  }

  Todo todos[MAX_TODOS] = {
      {"Write the layout pass", true},
      {"Add buttons and labels", true},
      {"Scroll containers", false},
      {"Text input", false},
  };
  int todoCount = 4;
  int created = 4;
  int filter = 0; // 0 all, 1 active, 2 done
  bool compact = false;
  const char *filters[] = {"All", "Active", "Done"};

  // set UI_SHOT=file.png to save one frame and quit (used to check the output)
  const char *shot = getenv("UI_SHOT");
  int frame = 0;

  while (!WindowShouldClose()) {
    int left = 0;
    for (int i = 0; i < todoCount; i++)
      if (!todos[i].done) left++;
    int toDelete = -1;

    BeginDrawing();
    ClearBackground(C_BG);
    UI_Begin();

    Box(.w = GROW(), .h = GROW(), .dir = UI_ROW) {

      // ---------------- sidebar: fixed width, full height
      Box(.w = PX(220), .h = GROW(), .pad = 16, .gap = 8, .bg = C_SURFACE) {
        Label("Lists", .fontSize = 24, .color = C_TEXT);
        for (int i = 0; i < 3; i++) {
          if (Button(filters[i], .w = GROW(), .bg = (filter == i) ? C_ACCENT : C_HOVER))
            filter = i;
        }
        Box(.h = GROW()) {} // spacer: pushes everything below to the bottom
        Checkbox("Compact rows", &compact, .color = C_MUTED);
        Label(TextFormat("%d of %d left", left, todoCount), .fontSize = 16, .color = C_MUTED);
      }

      // ---------------- main area: takes the remaining width
      Box(.w = GROW(), .h = GROW(), .pad = 24, .gap = 12) {

        Box(.w = GROW(), .dir = UI_ROW, .justify = UI_BETWEEN, .align = UI_CENTER) {
          Label("My Tasks", .fontSize = 36, .color = C_TEXT);
          if (Button("+ Add task") && todoCount < MAX_TODOS) {
            snprintf(todos[todoCount].text, sizeof todos[0].text, "New task %d", ++created);
            todos[todoCount].done = false;
            todoCount++;
          }
        }

        for (int i = 0; i < todoCount; i++) {
          if (filter == 1 && todos[i].done) continue;
          if (filter == 2 && !todos[i].done) continue;

          // the whole row is clickable; .idx makes each row's id unique
          Box(.id = "todo", .idx = i, .clickable = true, .w = GROW(), .dir = UI_ROW,
              .align = UI_CENTER, .gap = 12, .padX = 14, .padY = compact ? 6 : 14,
              .bg = C_SURFACE, .hoverBg = C_HOVER, .radius = 10) {
            if (UI_Sig().clicked) todos[i].done = !todos[i].done;

            Box(.w = PX(22), .h = PX(22), .radius = 11, .borderW = 2,
                .border = todos[i].done ? C_ACCENT : C_MUTED,
                .bg = todos[i].done ? C_ACCENT : C_BG) {}
            Label(todos[i].text, .color = todos[i].done ? C_MUTED : C_TEXT);
            Box(.w = GROW()) {} // spacer: pushes the delete button to the right edge
            if (Button("x", .id = "del", .idx = i, .bg = C_HOVER, .hoverBg = C_DANGER,
                       .padX = 12, .padY = 6))
              toDelete = i;
          }
        }
      }
    }

    UI_End(); // layout + draw happen here
    EndDrawing();

    if (toDelete >= 0) {
      memmove(&todos[toDelete], &todos[toDelete + 1], (todoCount - toDelete - 1) * sizeof(Todo));
      todoCount--;
    }

    if (shot && ++frame == 10) {
      Image img = LoadImageFromScreen();
      ExportImage(img, shot);
      UnloadImage(img);
      break;
    }
  }

  CloseWindow();
  return 0;
}
