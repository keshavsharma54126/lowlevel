#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
static struct termios oldt;

void exit_raw_mode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt); }
void enter_raw_mode() {
  tcgetattr(STDIN_FILENO, &oldt);
  atexit(exit_raw_mode);
  struct termios rawtermios = oldt;
  rawtermios.c_iflag &= ~(IXON | ICRNL);
  rawtermios.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &rawtermios);
}
int main() {
  enter_raw_mode();
  char ch;
  while (read(STDIN_FILENO, &ch, 1) == 1 && ch != 'q') {
    if (iscntrl(ch)) {
      printf("%d", ch);
    } else {
      printf("%d ('%c')/n", ch, ch);
    }
  }
  return 1;
}
