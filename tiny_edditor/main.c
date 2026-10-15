/************ Includes *************/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#define CTRL_KEY(k) ((k) & 0x1f)
#define NEW_ABUFF {NULL, 0}
#define TINY_VERSION "0.0.1"
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
struct edditorConfig {
  int edditorRows;
  int edditorCols;
  struct termios oldt;
};

struct edditorConfig E;
/************ terminal *************/
void drawRows(struct abuff *ab) {
  for (int i = 0; i < E.edditorRows; i++) {
    if (i == E.edditorRows / 3) {
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
    // clearning screen below the cursor with excape sequence
    abuffAppend(ab, "\x1b[K", 3);

    if (i < E.edditorRows - 1) {
      abuffAppend(ab, "\r\n", 2);
    }
  }
}
void edditorRefreshScreen() {

  struct abuff ab = NEW_ABUFF;
  // hide the cursor with the below escape sequence
  abuffAppend(&ab, "\x1b[?25l", 6);
  // then positioning the cursor to the start at
  abuffAppend(&ab, "\x1b[H", 3);

  // here we willl draw the tildes to make it look
  // like we are in an edditor like feeling
  drawRows(&ab);
  abuffAppend(&ab, "\x1b[H", 3);
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
char edditorReadKey() {
  int nread;
  char ch;
  while ((nread = read(STDIN_FILENO, &ch, 1)) != 1) {
    if (nread == -1 && errno == EAGAIN)
      die("faield to read character");
  }
  return ch;
}

void edditorProcessKey() {
  char ch = edditorReadKey();
  switch (ch) {
  case (CTRL_KEY('q')):
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    exit(1);
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
    if (read(STDIN_FILENO, &buff, 1) != 1) {
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
void initEdditor() {
  if (getWindowSize(&E.edditorRows, &E.edditorCols) == -1) {
    printf("no of cols: %d \r\n no of rows: %d \r\n", E.edditorCols,
           E.edditorRows);
    die("getScreenSize");
  }
}
/************** init ****************/
int main() {
  // first we will enter raw_mode which will
  // basically make sure that the iput goes without
  // enter continuous , disabling, ctrl+z, etc etc
  // so that it will behave like a text edditor
  enter_raw_mode();
  initEdditor();
  while (1) {
    edditorRefreshScreen();
    edditorProcessKey();
  }
  exit_raw_mode();
  return 1;
}
