/************ Includes *************/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)
/************ data *****************/
static struct termios oldt;

/************ terminal *************/

void edditorRefreshScreen() {
  // clearning screen the whole screen with excape
  write(STDOUT_FILENO, "\x1b[2J", 4);
  // then positioning the cursor to the start at
  write(STDOUT_FILENO, "\x1b[H", 3);
}
void die(char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}

void exit_raw_mode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt) == -1) {
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
  tcgetattr(STDIN_FILENO, &oldt);
  /* when main will exit it will basically call
   * exit_raw_mode so that the terminal settings go
   * back to original
   */

  atexit(exit_raw_mode);
  // creatinga new temp termios to change the
  // current settings cause oldt has bee declared a
  // global for basically changing the terminal
  // settings to original after we exit the mode
  struct termios rawtermios = oldt;

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
    write(STDOUT_FILENO, "\x1b[", 4);
    break;
  default:
    write(STDOUT_FILENO, &ch, 1);
    break;
  }
  return;
}

/************** init ****************/
int main() {
  // first we will enter raw_mode which will
  // basically make sure that the iput goes without
  // enter continuous , disabling, ctrl+z, etc etc
  // so that it will behave like a text edditor
  enter_raw_mode();
  while (1) {
    edditorProcessKey();
  }
  exit_raw_mode();
  return 1;
}
