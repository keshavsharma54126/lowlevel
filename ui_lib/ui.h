// ui.h - bare-bones immediate-mode UI with autolayout, on raylib only.
//
// You declare boxes every frame. Nothing is drawn while you declare:
// UI_End() sizes everything, positions everything, then draws.
//
//   UI_Begin();
//   Box(.w = GROW(), .dir = UI_ROW, .gap = 8) {
//     Label("Hello");
//     if (Button("Click me")) { ... }
//   }
//   UI_End();
//
// Do not `break` or `return` out of a Box(...) { } block: the box would never close.
#ifndef UI_H
#define UI_H

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "raylib.h"

#define UI_MAX_BOXES 4096
#define UI_MAX_DEPTH 64
#define UI_TEXT_ARENA (64 * 1024)

// ------------------------------------------------------------------ types
typedef enum { UI_FIT, UI_GROW, UI_FIXED, UI_PERCENT } UI_SizeKind;
typedef struct {
  UI_SizeKind kind;
  float value;
} UI_Size;

#define FIT() ((UI_Size){UI_FIT, 0})                 // as big as the content (default)
#define GROW() ((UI_Size){UI_GROW, 0})               // take the leftover space
#define PX(n) ((UI_Size){UI_FIXED, (float)(n)})      // exact pixels
#define PCT(n) ((UI_Size){UI_PERCENT, (n) / 100.0f}) // % of the parent's inner size

typedef enum { UI_COLUMN, UI_ROW } UI_Dir;                         // flex-direction
typedef enum { UI_START, UI_CENTER, UI_END, UI_BETWEEN } UI_Align; // BETWEEN: justify only

// Every property a box can have. Zero means "default", so you only set what you need.
typedef struct {
  int _; // placeholder so Box() with no arguments compiles

  // identity + interaction
  const char *id; // needed for clickable boxes (Button uses its label)
  int idx;        // extra number for ids inside loops
  bool clickable;

  // size
  UI_Size w, h;

  // how children are laid out
  UI_Dir dir;         // UI_COLUMN (default) or UI_ROW
  float pad;          // padding on all sides
  float padX, padY;   // override pad per axis
  float gap;          // space between children
  UI_Align justify;   // along dir
  UI_Align align;     // across dir

  // look
  Color bg, hoverBg, activeBg;
  Color border; // drawn only when bg is also set
  float borderW;
  float radius;

  // text (makes the box a text leaf)
  const char *text;
  float fontSize;
  Color color;
  UI_Align textAlign;
} UI_Style;

typedef struct {
  bool hovered, pressed, clicked;
} UI_Signal;

typedef struct {
  UI_Style s;
  unsigned id;
  int parent, first, last, next, count; // tree links (indices into ui.boxes)
  UI_Size sz[2];                        // {w, h} so layout code can loop over axes
  float pos[2], size[2];                // final rectangle
  const char *text;
  UI_Signal sig;
} UI_Box;

// ------------------------------------------------------------------ state
static struct {
  UI_Box boxes[UI_MAX_BOXES]; // rebuilt every frame
  int boxCount;
  int stack[UI_MAX_DEPTH]; // currently open boxes
  int depth;
  char textArena[UI_TEXT_ARENA]; // copies of label strings for this frame
  int textUsed;
  unsigned hot;     // box under the mouse (found while drawing the previous frame)
  unsigned nextHot; // being found while drawing this frame
  unsigned active;  // box the mouse button went down on
  Font font;
  bool hasFont;
} ui;

static void UI_SetFont(Font f) {
  ui.font = f;
  ui.hasFont = true;
}

// ------------------------------------------------------------------ helpers
static unsigned UI_Hash(const char *s, int idx) {
  unsigned h = 2166136261u; // FNV-1a
  for (; *s; s++) {
    h ^= (unsigned char)*s;
    h *= 16777619u;
  }
  h ^= (unsigned)idx * 2654435761u;
  return h ? h : 1; // 0 means "no id"
}

static float UI_PadX(const UI_Box *b) { return b->s.padX ? b->s.padX : b->s.pad; }
static float UI_PadY(const UI_Box *b) { return b->s.padY ? b->s.padY : b->s.pad; }
static float UI_Spacing(float fontSize) { return ui.hasFont ? 0 : fontSize / 10; }

static void UI_Fill(Rectangle r, float radius, Color c) {
  if (r.width <= 0 || r.height <= 0 || c.a == 0) return;
  if (radius <= 0) {
    DrawRectangleRec(r, c);
    return;
  }
  float shortest = r.width < r.height ? r.width : r.height;
  float roundness = 2 * radius / shortest;
  if (roundness > 1) roundness = 1;
  DrawRectangleRounded(r, roundness, 12, c);
}

// ------------------------------------------------------------------ declare
static UI_Signal UI_BoxBegin(UI_Style s) {
  assert(ui.boxCount < UI_MAX_BOXES && ui.depth < UI_MAX_DEPTH);

  int bi = ui.boxCount++;
  UI_Box *b = &ui.boxes[bi];
  memset(b, 0, sizeof *b);
  b->s = s;
  b->sz[0] = s.w;
  b->sz[1] = s.h;
  b->first = b->last = b->next = -1;
  b->parent = ui.depth ? ui.stack[ui.depth - 1] : -1;

  if (b->parent >= 0) { // append to the parent's child list
    UI_Box *p = &ui.boxes[b->parent];
    if (p->last >= 0) ui.boxes[p->last].next = bi;
    else p->first = bi;
    p->last = bi;
    p->count++;
  }

  if (s.text) { // copy, so TextFormat() strings survive until UI_End
    int len = (int)strlen(s.text) + 1;
    assert(ui.textUsed + len <= UI_TEXT_ARENA);
    b->text = memcpy(ui.textArena + ui.textUsed, s.text, len);
    ui.textUsed += len;
    if (b->s.fontSize <= 0) b->s.fontSize = 18;
    if (b->s.color.a == 0) b->s.color = RAYWHITE;
  }

  if (s.clickable) {
    const char *name = s.id ? s.id : (s.text ? s.text : "box");
    b->id = UI_Hash(name, (s.id || s.text) ? s.idx : bi);
    b->sig.hovered = (ui.hot == b->id);
    if (b->sig.hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ui.active = b->id;
    b->sig.pressed = (ui.active == b->id);
    b->sig.clicked = b->sig.hovered && b->sig.pressed && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
  }

  ui.stack[ui.depth++] = bi;
  return b->sig;
}

// Layout pass 1 (bottom-up): when a box closes, all its children are known,
// so its "fit" size can be computed.
static void UI_BoxEnd(void) {
  assert(ui.depth > 0);
  UI_Box *b = &ui.boxes[ui.stack[--ui.depth]];
  int m = (b->s.dir == UI_ROW) ? 0 : 1; // main axis: 0 = x, 1 = y
  int c = 1 - m;                        // cross axis
  float pad[2] = {UI_PadX(b), UI_PadY(b)};
  float content[2] = {0, 0};

  if (b->text) {
    Vector2 t = MeasureTextEx(ui.font, b->text, b->s.fontSize, UI_Spacing(b->s.fontSize));
    content[0] = t.x;
    content[1] = t.y;
  }
  for (int ci = b->first; ci >= 0; ci = ui.boxes[ci].next) {
    UI_Box *ch = &ui.boxes[ci];
    content[m] += ch->size[m];                               // children add up along the main axis
    if (ch->size[c] > content[c]) content[c] = ch->size[c];  // biggest child wins on the cross axis
  }
  if (b->count > 1) content[m] += b->s.gap * (b->count - 1);

  for (int a = 0; a < 2; a++) {
    if (b->sz[a].kind == UI_FIXED) b->size[a] = b->sz[a].value;
    else if (b->sz[a].kind == UI_PERCENT) b->size[a] = 0; // known only once the parent is sized
    else b->size[a] = content[a] + 2 * pad[a];            // FIT, and the minimum for GROW
  }
}

// Signal of the box you are currently inside:  Box(.clickable = true) { if (UI_Sig().clicked) ... }
static UI_Signal UI_Sig(void) { return ui.boxes[ui.stack[ui.depth - 1]].sig; }

// ------------------------------------------------------------------ layout pass 2 (top-down)
// The box's own size is final here. Resolve PERCENT and GROW children, then place them.
static void UI_Layout(int bi) {
  UI_Box *b = &ui.boxes[bi];
  int m = (b->s.dir == UI_ROW) ? 0 : 1;
  int c = 1 - m;
  float pad[2] = {UI_PadX(b), UI_PadY(b)};
  float inner[2] = {b->size[0] - 2 * pad[0], b->size[1] - 2 * pad[1]};

  float used = 0;
  int growCount = 0;
  for (int ci = b->first; ci >= 0; ci = ui.boxes[ci].next) {
    UI_Box *ch = &ui.boxes[ci];
    for (int a = 0; a < 2; a++)
      if (ch->sz[a].kind == UI_PERCENT) ch->size[a] = inner[a] * ch->sz[a].value;
    if (ch->sz[c].kind == UI_GROW) ch->size[c] = inner[c]; // GROW across = fill
    if (ch->sz[m].kind == UI_GROW) growCount++;
    used += ch->size[m];
  }
  if (b->count > 1) used += b->s.gap * (b->count - 1);

  float leftover = inner[m] - used;
  if (growCount > 0 && leftover > 0) { // GROW along = share what is left
    for (int ci = b->first; ci >= 0; ci = ui.boxes[ci].next)
      if (ui.boxes[ci].sz[m].kind == UI_GROW) ui.boxes[ci].size[m] += leftover / growCount;
    leftover = 0;
  }

  float cursor = pad[m];
  float between = b->s.gap;
  if (leftover > 0) {
    if (b->s.justify == UI_CENTER) cursor += leftover / 2;
    else if (b->s.justify == UI_END) cursor += leftover;
    else if (b->s.justify == UI_BETWEEN && b->count > 1) between += leftover / (b->count - 1);
  }

  for (int ci = b->first; ci >= 0; ci = ui.boxes[ci].next) {
    UI_Box *ch = &ui.boxes[ci];
    float offset = 0;
    if (b->s.align == UI_CENTER) offset = (inner[c] - ch->size[c]) / 2;
    else if (b->s.align == UI_END) offset = inner[c] - ch->size[c];

    ch->pos[m] = b->pos[m] + cursor;
    ch->pos[c] = b->pos[c] + pad[c] + offset;
    cursor += ch->size[m] + between;
    UI_Layout(ci);
  }
}

// ------------------------------------------------------------------ draw
static void UI_Draw(int bi) {
  UI_Box *b = &ui.boxes[bi];
  Rectangle r = {b->pos[0], b->pos[1], b->size[0], b->size[1]};
  Color bg = b->s.bg;

  if (b->id) {
    // children are drawn after parents, so the deepest clickable box under the mouse wins
    if (CheckCollisionPointRec(GetMousePosition(), r)) ui.nextHot = b->id;
    if (b->id == ui.hot) {
      if (ui.active == b->id && b->s.activeBg.a) bg = b->s.activeBg;
      else if (b->s.hoverBg.a) bg = b->s.hoverBg;
    }
  }

  if (b->s.borderW > 0 && b->s.border.a && bg.a) {
    float bw = b->s.borderW;
    UI_Fill(r, b->s.radius, b->s.border);
    Rectangle in = {r.x + bw, r.y + bw, r.width - 2 * bw, r.height - 2 * bw};
    UI_Fill(in, b->s.radius - bw, bg);
  } else {
    UI_Fill(r, b->s.radius, bg);
  }

  if (b->text) {
    float spacing = UI_Spacing(b->s.fontSize);
    Vector2 t = MeasureTextEx(ui.font, b->text, b->s.fontSize, spacing);
    float innerW = r.width - 2 * UI_PadX(b);
    float x = r.x + UI_PadX(b);
    if (b->s.textAlign == UI_CENTER) x += (innerW - t.x) / 2;
    else if (b->s.textAlign == UI_END) x += innerW - t.x;
    float y = r.y + (r.height - t.y) / 2;
    DrawTextEx(ui.font, b->text, (Vector2){x, y}, b->s.fontSize, spacing, b->s.color);
  }

  for (int ci = b->first; ci >= 0; ci = ui.boxes[ci].next) UI_Draw(ci);
}

// ------------------------------------------------------------------ frame
static void UI_Begin(void) {
  ui.boxCount = 0;
  ui.depth = 0;
  ui.textUsed = 0;
  ui.hot = ui.nextHot;
  ui.nextHot = 0;
  if (!ui.hasFont) ui.font = GetFontDefault();
  // root box = the whole window
  UI_BoxBegin((UI_Style){.w = PX(GetScreenWidth()), .h = PX(GetScreenHeight())});
}

static void UI_End(void) {
  UI_BoxEnd(); // close root
  assert(ui.depth == 0);
  UI_Layout(0);
  UI_Draw(0);
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) ui.active = 0;
}

// ------------------------------------------------------------------ components
// Everything below is just boxes with some properties pre-filled.

static void UI_Label(const char *text, UI_Style s) {
  s.text = text;
  UI_BoxBegin(s);
  UI_BoxEnd();
}

static bool UI_Button(const char *label, UI_Style s) {
  s.text = label;
  s.clickable = true;
  s.textAlign = UI_CENTER;
  if (!s.pad && !s.padX && !s.padY) {
    s.padX = 16;
    s.padY = 10;
  }
  if (!s.radius) s.radius = 8;
  if (!s.bg.a) s.bg = (Color){99, 102, 241, 255};
  if (!s.hoverBg.a) s.hoverBg = ColorBrightness(s.bg, 0.15f);
  if (!s.activeBg.a) s.activeBg = ColorBrightness(s.bg, -0.15f);
  UI_Signal sig = UI_BoxBegin(s);
  UI_BoxEnd();
  return sig.clicked;
}

static bool UI_Checkbox(const char *label, bool *value, UI_Style s) {
  s.clickable = true;
  s.dir = UI_ROW;
  s.align = UI_CENTER;
  if (!s.id) s.id = label;
  if (!s.gap) s.gap = 10;
  UI_Signal sig = UI_BoxBegin(s);
  if (sig.clicked) *value = !*value;

  Color accent = {99, 102, 241, 255};
  UI_BoxBegin((UI_Style){.w = PX(20), .h = PX(20), .radius = 6, .borderW = 2,
                         .border = *value ? accent : (Color){100, 116, 139, 255},
                         .bg = *value ? accent : (Color){15, 23, 42, 255}});
  UI_BoxEnd();
  UI_Label(label, (UI_Style){.fontSize = s.fontSize, .color = s.color});

  UI_BoxEnd();
  return sig.clicked;
}

// Block syntax: the for-loop runs its body once, opening the box before and closing it after.
#define Box(...)                                                                         \
  for (int ui_once_ = (UI_BoxBegin((UI_Style){._ = 0, __VA_ARGS__}), 0); !ui_once_;      \
       ui_once_ = (UI_BoxEnd(), 1))

#define Label(text, ...) UI_Label((text), (UI_Style){._ = 0, __VA_ARGS__})
#define Button(text, ...) UI_Button((text), (UI_Style){._ = 0, __VA_ARGS__})
#define Checkbox(text, value, ...) UI_Checkbox((text), (value), (UI_Style){._ = 0, __VA_ARGS__})

#endif // UI_H
