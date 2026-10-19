/************ Includes *************/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#define CTRL_KEY(k) ((k) & 0x1f)
#define NEW_ABUFF {NULL, 0}
#define TINY_VERSION "0.0.1"

enum edditorKeys {
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  PAGE_UP,
  PAGE_DOWN,
  HOME,
  END,
  DEL
};

/** dyanmic strings**/
struct abuff {
  char *str;
  int len;
};

void abuffAppend(struct abuff *ab, char *s, int len) {
  char *new = realloc(ab->str, ab->len + len);
  if (new == NULL) {
    return;
  }
  memcpy(&new[ab->len], s, len);
  ab->str = new;
  ab->len += len;
}

void abuffFree(struct abuff *ab) {
  free(ab->str);
  ab->str = NULL;
  ab->len = 0;
}
/************ data *****************/
typedef struct erow {
  char *chars;
  int size;
} erow;
struct edditorConfig {
  int cx;
  int cy;
  int rowoff;
  int colOff;
  int edditorRows;
  int edditorCols;
  int noOfRows;
  erow *rows;
  struct termios oldt;
};

struct edditorConfig E;
/************ terminal *************/
void drawRows(struct abuff *ab) {
  for (int i = 0; i < E.edditorRows; i++) {
    int filerow = i + E.rowoff;
    if (filerow >= E.noOfRows) {
      if (E.noOfRows == 0 && i == E.edditorRows / 3) {
        char welcome[80];
        int welcomeLen = snprintf(welcome, sizeof(welcome),
                                  "Tiny Editor -- version %s", TINY_VERSION);
        if (welcomeLen > E.edditorCols) {
          welcomeLen = E.edditorCols;
        }
        int padding = (E.edditorCols - welcomeLen) / 2;
        if (padding) {
          abuffAppend(ab, "~", 1);
          padding--;
        }
        while (padding--) {
          abuffAppend(ab, " ", 1);
        }
        abuffAppend(ab, welcome, welcomeLen);
      } else {
        abuffAppend(ab, "~", 1);
      }
    } else {
      int len = E.rows[filerow].size - E.colOff;
      if (len < 0)
        len = 0;
      if (E.rows[filerow].size > E.edditorCols)
        len = E.edditorCols;
      abuffAppend(ab, &E.rows[filerow].chars[E.colOff], len);
    }

    // clearning screen below the cursor with excape sequence
    abuffAppend(ab, "\x1b[K", 3);

    if (i < E.edditorRows - 1) {
      abuffAppend(ab, "\r\n", 2);
    }
  }
}

void edditorScroll() {
  if (E.cy < E.rowoff) {
    E.rowoff = E.cy;
  }
  if (E.cy >= E.rowoff + E.edditorRows) {
    E.rowoff = E.cy - E.edditorRows + 1;
  }

  if (E.cx < E.colOff) {
    E.colOff = E.cx;
  }
  if (E.cx >= E.colOff + E.edditorCols) {
    E.colOff = E.cx - E.edditorCols + 1;
  }
}
void edditorRefreshScreen() {
  edditorScroll();
  struct abuff ab = NEW_ABUFF;
  // hide the cursor with the below escape sequence
  abuffAppend(&ab, "\x1b[?25l", 6);
  // then positioning the cursor to the start at
  abuffAppend(&ab, "\x1b[H", 3);
  // here we willl draw the tildes to make it look
  // like we are in an edditor like feeling

  drawRows(&ab);
  // normalize the cx and cy from 0 based to 1
  // based indexing
  char buff[32];
  snprintf(buff, sizeof(buff), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1,
           (E.cx - E.colOff + 1));

  abuffAppend(&ab, buff, strlen(buff));
  abuffAppend(&ab, "\x1b[?25h", 6);
  write(STDOUT_FILENO, ab.str, ab.len);
  abuffFree(&ab);
}
void die(char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}

void exit_raw_mode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.oldt) == -1) {
    die("tcsetattr");
  }
}

void enter_raw_mode() {
  /* tcgetatrr basically takes a file descriptor and puts all the terminal
  settigns in the termios struct which is basically all the settings of
  the terminal

  standard file descriptors are given below

    The standard file descriptors are:

    Constant                  Value             Purpose

    STDIN_FILENO                0               Standard input

    STDOUT_FILENO               1               Standard output

    STDERR_FILENO               2               Standard error

  */
  tcgetattr(STDIN_FILENO, &E.oldt);
  /* when main will exit it will basically call
   * exit_raw_mode so that the terminal settings go
   * back to original
   */

  atexit(exit_raw_mode);
  // creatinga new temp termios to change the
  // current settings cause oldt has bee declared a
  // global for basically changing the terminal
  // settings to original after we exit the mode
  struct termios rawtermios = E.oldt;

  rawtermios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  /*inverting the c_iflag which will disable input
   * input controlf flags of ctrl+s and ctrl+q
   */
  rawtermios.c_iflag &= ~(IXON | ICRNL);
  /* turn of output processing flags which like
   * which convert '\n' to '\r\n'
   * it is also known as carriage return
   */
  rawtermios.c_oflag &= ~(OPOST);

  /*set the bit flag for characters to cs8 which
   * will make the characters we type 8bit
   */

  rawtermios.c_cflag |= (CS8);
  /*inverting the c_lflag which will disable local
   * flags for terminal settings
   *  ECHO — disable displaying typed characters
   *  ICANON — disable canonical mode(canonical
   *  mode buffers you input until you presss enter)
   *  ISIG — disable terminal-generated signals
      Normally, the terminal interprets certain control characters as
   signals:

      Ctrl+C generates SIGINT.

      Ctrl+\ generates SIGQUIT.

      Ctrl+Z generates SIGTSTP on typical configurations.

      IEXTEN — disable implementation-defined input extensions

   */

  rawtermios.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

  rawtermios.c_cc[VMIN] = 0;
  rawtermios.c_cc[VTIME] = 1;
  // then set the new termios settings by flushing
  // it immidiately

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &rawtermios)) {
    die("tcsetattr");
  }
}

void moveCursor(int ch) {
  erow *row = (E.cy >= E.noOfRows) ? NULL : &E.rows[E.cy];
  switch (ch) {
  case ARROW_UP:
    if (E.cy != 0)
      E.cy--;
    break;
  case ARROW_LEFT:
    if (E.cx == 0) {
      E.cy--;
      row = (E.cy >= E.noOfRows) ? NULL : &E.rows[E.cy];
      E.cx = row->size;
    }
    if (E.cx != 0)
      E.cx--;
    break;
  case ARROW_DOWN:
    if (E.cy < E.noOfRows)
      E.cy++;
    break;
  case ARROW_RIGHT:

    if (E.cx == row->size) {
      E.cy++;
      E.cx = 0;
    }
    if (row && E.cx < row->size)
      E.cx++;
    break;
  }
  row = (E.cy >= E.noOfRows) ? NULL : &E.rows[E.cy];
  int rowlen = row ? row->size : 0;
  if (E.cx > rowlen) {
    E.cx = rowlen;
  }
}

int edditorReadKey() {
  int nread;
  char ch;
  while ((nread = read(STDIN_FILENO, &ch, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      die("faield to read character");
  }
  char seq[3];
  if (ch == '\x1b') {
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';

    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1)
          return '\x1b';
        else if (seq[2] == '~') {
          switch (seq[1]) {
          case ('1'):
            return HOME;
          case ('4'):
            return END;
          case ('8'):
            return END;
          case ('7'):
            return HOME;
          case ('5'):
            return PAGE_UP;
          case ('6'):
            return PAGE_DOWN;
          case ('3'):
            return DEL;
          }
        }
      } else {
        switch (seq[1]) {
        case ('H'):
          return HOME;
        case ('F'):
          return END;
        case ('A'):
          return ARROW_UP;
        case ('B'):
          return ARROW_DOWN;
        case ('C'):
          return ARROW_RIGHT;
        case ('D'):
          return ARROW_LEFT;
        }
      }
    } else if (seq[0] == 'O') {
      switch (seq[1]) {
      case ('H'):
        return HOME;
      case ('F'):
        return END;
      }
    }
    return '\x1b';
  }

  return ch;
}

void edditorProcessKey() {
  int ch = edditorReadKey();
  switch (ch) {
  case (CTRL_KEY('q')):
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    exit(1);
    break;
  case HOME:
    E.cx = 0;
    break;
  case END:
    E.cx = (E.cy < E.edditorRows) ? E.rows[E.cy].size : 0;
    break;
  case PAGE_UP:
  case PAGE_DOWN:
    int moves = E.edditorRows;
    while (moves--) {
      moveCursor(ch == PAGE_DOWN ? ARROW_DOWN : ARROW_UP);
    }
    break;
  case ARROW_UP:
  case ARROW_LEFT:
  case ARROW_DOWN:
  case ARROW_RIGHT:
    moveCursor(ch);
    break;
  case (CTRL_KEY('\n')):
    write(STDOUT_FILENO, "\x1b[", 2);
    break;
  default:
    write(STDOUT_FILENO, &ch, 1);
    break;
  }
  return;
}

int getCursorPosition(int *rows, int *cols) {
  char buff[32];
  unsigned int i = 1;
  if (write(STDOUT_FILENO, "\x1b[9n", 4) != 4) {
    return -1;
  }
  printf("\r\n");

  while (i < sizeof(buff)) {
    if (read(STDIN_FILENO, &buff[i], 1) != 1) {
      break;
    }
    if (buff[i] == 'R') {
      break;
    }
    i++;
  }
  buff[i] = '\0';
  if (buff[0] != '\x1b' || buff[1] != '[') {
    return -1;
  }
  if (sscanf(&buff[2], "%d;%d", rows, cols) != 2) {
    return -1;
  }
  printf("\r\n &buff[1]: %s\r\n", &buff[1]);
  edditorReadKey();
  return 0;
}
int getWindowSize(int *rows, int *cols) {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
      return -1;
    getCursorPosition(rows, cols);
    return -1;
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

// append rows to the edditor file rows
void appendRows(char *s, size_t len) {
  E.rows = realloc(E.rows, sizeof(erow) * (E.noOfRows + 1));
  int currRow = E.noOfRows;
  E.rows[currRow].size = len;
  E.rows[currRow].chars = malloc(len + 1);
  memcpy(E.rows[currRow].chars, s, len);
  E.rows[currRow].chars[len] = '\0';
  E.noOfRows++;
}
// open a file
void openFileInEdditor(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    die("fopen");
  }
  char *line = NULL;
  size_t linecap = 0;
  size_t lineLength;
  while ((lineLength = getline(&line, &linecap, fp)) != -1) {
    while (lineLength > 0 &&
           (line[lineLength - 1] == '\r' || line[lineLength - 1] == '\n')) {
      lineLength--;
    }
    appendRows(line, lineLength);
  }
  free(line);
  fclose(fp);
}
void initEdditor() {
  // initialize x and y corordinates of the cursor
  // to 0;
  E.cx = 0;
  E.cy = 0;
  E.noOfRows = 0;
  E.colOff = 0;
  E.rows = NULL;
  if (getWindowSize(&E.edditorRows, &E.edditorCols) == -1) {
    printf("no of cols: %d \r\n no of rows: %d \r\n", E.edditorCols,
           E.edditorRows);
    die("getScreenSize");
  }
}
/************** init ****************/
int main(int argc, char *argv[]) {
  // first we will enter raw_mode which will
  // basically make sure that the iput goes without
  // enter continuous , disabling, ctrl+z, etc etc
  // so that it will behave like a text edditor

  enter_raw_mode();
  initEdditor();
  if (argc >= 2) {
    openFileInEdditor(argv[1]);
  }
  while (1) {
    edditorRefreshScreen();
    edditorProcessKey();
  }
  exit_raw_mode();
  return 1;
}
