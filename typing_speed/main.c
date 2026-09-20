#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
// sample text
typedef struct {
  int hours;
  int minutes;
  int seconds;
} Timer;

char *return_timer_string(Timer *timer) {
  static char timer_string[20];
  snprintf(timer_string, sizeof(timer_string), "%2d:%2d:%2d", timer->hours,
           timer->minutes, timer->seconds);
  return timer_string;
}
char *calculate_timer_string(Timer *timer, int seconds_elapsed) {
  static char timer_string[20];
  int total_seconds =
      timer->hours * 3600 + timer->minutes * 60 + timer->seconds;

  total_seconds -= seconds_elapsed;
  int h, m, s;
  if (total_seconds <= 0) {
    h = 0;
    m = 0;
    s = 0;
  } else {
    h = total_seconds / 3600;
    m = (total_seconds % 3600) / 60;
    s = total_seconds % 60;
  }
  snprintf(timer_string, sizeof(timer_string), "%2d:%2d:%2d", h, m, s);
  return timer_string;
}
char *txt =
    "\nThe think tank of China’s People’s Liberation Army Rocket Force "
    "recently "
    "recruited 13 Chinese technicians\nfrom private companies, PLA Daily "
    "reported on Saturday."
    "\nZhang Hao and 12 other science and technology experts received letters"
    " of appointment at the founding ceremony of "
    "\nthe PLA Rocket Force national defense science and technology experts "
    "panel, according to a report published by the"
    "\nPLA Daily on Saturday."
    "\nHonored as rocket force science and technology experts,” Zhang and his"
    " fellow experts from private companies will "
    "\nserve as members of the PLA Rocket Force think tank, which will conduct "
    " research into fields like overall design of "
    "\nthe missiles, missile launching and network system technology for five"
    " years."
    "\nThe experts will enjoy the same treatment as their counterparts"
    " from State - owned firms,the report said."
    "\nThe PLA Daily said that this marks a new development in deepening "
    "military - civilian integration in China,which "
    "\ncould make science and technology innovation better contribute to "
    "the"
    "\nenhancement of the force’s combat capabilities \n";

//  Color Escape Combinations
const int FRAME_RATE = 120;
const double TICK_RATE = (float)(1.00 / FRAME_RATE);

char *splash_text =
    "\n"
    "╔══════════════════════════════════════════════════════════════╗\n"
    "║                                                              ║\n"
    "║   ████████╗██╗   ██╗██████╗ ███████╗    ████████╗ █████╗     ║\n"
    "║   ╚══██╔══╝╚██╗ ██╔╝██╔══██╗██╔════╝    ╚══██╔══╝██╔══██╗    ║\n"
    "║      ██║    ╚████╔╝ ██████╔╝█████╗         ██║   ███████║    ║\n"
    "║      ██║     ╚██╔╝  ██╔═══╝ ██╔══╝         ██║   ██╔══██║    ║\n"
    "║      ██║      ██║   ██║     ███████╗       ██║   ██║  ██║    ║\n"
    "║      ╚═╝      ╚═╝   ╚═╝     ╚══════╝       ╚═╝   ╚═╝  ╚═╝    ║\n"
    "║                                                              ║\n"
    "║                    ⚡ TYPE // REACT // WIN ⚡                ║\n"
    "║                                                              ║\n"
    "╚══════════════════════════════════════════════════════════════╝\n";

// Reset All Styles
#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"

// Foreground / Text Colors
#define FG_BLACK "\x1b[30m"
#define FG_RED "\x1b[31m"
#define FG_GREEN "\x1b[32m"
#define FG_YELLOW "\x1b[33m"
#define FG_BLUE "\x1b[34m"
#define FG_MAGENTA "\x1b[35"
#define FG_CYAN "\x1b[36m"
#define FG_WHITE "\x1b[37m"

// Background Colors
#define BG_BLACK "\x1b[40m"
#define BG_RED "\x1b[41m"
#define BG_GREEN "\x1b[42m"
#define BG_YELLOW "\x1b[43m"
#define BG_BLUE "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN "\x1b[46m"
#define BG_WHITE "\x1b[47m"

// Always use RESET to go back to normal text
#define RESET "\x1b[0m"

// Standard Simple Text Colors
#define TXT_BLACK "\x1b[30m"
#define TXT_RED "\x1b[31m"
#define TXT_GREEN "\x1b[32m"
#define TXT_YELLOW "\x1b[33m"
#define TXT_BLUE "\x1b[34m"
#define TXT_MAGENTA "\x1b[35m"
#define TXT_CYAN "\x1b[36m"
#define TXT_WHITE "\x1b[37m"

#define CLEAR_SCREEN "\x1b[2J" // Clears entire screen using the ACTIVE color
#define CURSOR_HOME "\x1b[1;1H"
#define SET_COLOR(bg, fg) "\x1b[" bg ";" fg "m"

void get_curr_winsize(struct winsize *w) {
  ioctl(STDOUT_FILENO, TIOCGWINSZ, w);
}
double get_time_in_seconds() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.00;
}

struct winsize *w;

void clear_screen() { printf("%s", CLEAR_SCREEN); }
void clear_background() {
  printf("%s", RESET);
  fflush(stdout);
}

void set_background_color(const char *bg_color) {
  printf("%s%s%s", bg_color, CLEAR_SCREEN, CURSOR_HOME);
  fflush(stdout);
}

void move_cursor(int row, int col) {
  printf("\x1b[%d;%dH", row, col);

  fflush(stdout);
}
void print_with_color(const char *text_color, const char *text_bg_color,
                      bool next_line, const char *text) {
  if (text == NULL) {
    printf("text can not be empty");
    exit(1);
  }
  if (text_bg_color == NULL) {
    text_bg_color = "";
  }
  if (next_line) {
    printf("%s%s%s%s\n", text_color, text_bg_color, text, RESET);
  } else {
    printf("%s%s%s%s", text_color, text_bg_color, text, RESET);
  }
  fflush(stdout);
}

void show_start_menu(int *no_of_words, int *time_in_seconds) {
  clear_screen();
  print_with_color(TXT_CYAN, NULL, true, splash_text);
  // print_with_color(TXT_CYAN, NULL, "Please enter the no of words: ");
  // scanf("%d", no_of_words);
  print_with_color(TXT_CYAN, NULL, true,
                   "Please enter the time limit in seconds: ");
  scanf("%d", time_in_seconds);
  clear_screen();
}

Timer calculate_timer(int *time_in_seconds) {
  // calcuate the minutes and seconds and start printing time
  if (time_in_seconds == NULL || *time_in_seconds == 0) {
    print_with_color(
        TXT_RED, NULL, true,
        "Time can not be null or 0, please provide a valid amount of time\n");
    exit(1);
  }
  int total_seconds = *time_in_seconds;
  int h = total_seconds / 3600;
  int m = (total_seconds % 3600) / 60;
  int s = total_seconds % 60;
  return (Timer){.hours = h, .minutes = m, .seconds = s};
}
void start_timer(Timer *timer, int seconds_elapsed) {
  char *timer_string = calculate_timer_string(timer, seconds_elapsed);
  int row = 1;
  int col = 1;
  move_cursor(row, col);
  print_with_color(TXT_GREEN, NULL, true, timer_string);
}
char getch() {
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  char ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}
void get_position(char *txt, int index, int *r, int *c, int start_row,
                  int start_col) {
  *r = start_row;
  *c = start_col;
  for (int j = 0; j < index && txt[j]; j++) {
    if (txt[j] == '\n') {
      (*r)++;
    } else {
      (*c)++;
    }
  }
}
void print_progress(char *txt, char *progress) {
  struct winsize w;
  get_curr_winsize(&w);
  int txt_row = 2;
  int txt_col = 1;
  move_cursor(txt_row, txt_col);
  print_with_color(TXT_WHITE, NULL, false, txt);
  // print_with_color(TXT_GREEN, NULL, false, progress);
  for (int i = 0; i < strlen(progress); i++) {

    int r, c;
    get_position(txt, i, &r, &c, txt_row, txt_col);
    move_cursor(r, c);
    char buf[2] = {txt[i], '\0'};
    if (txt[i] == progress[i]) {
      print_with_color(TXT_GREEN, NULL, false, buf);
    } else {
      buf[0] = txt[i];
      print_with_color(TXT_RED, NULL, false, buf);
    }
  }
}
void start_test(int *words, int *time, double *lag, bool *running) {
  double last_time = get_time_in_seconds();
  double start_time = get_time_in_seconds();
  Timer timer = calculate_timer(time);
  int seconds_elapsed = 0;
  double seconds_accumlator = 0.0;
  int i = 0;
  int text_length = strlen(txt);
  char *progress = malloc(text_length);
  while (*running) {
    char ch = getch();
    progress[i] = ch;
    // strcat(progress, chi);
    i++;

    double current_time = get_time_in_seconds();
    double time_elapsed = current_time - last_time;

    last_time = current_time;

    *lag += time_elapsed;
    seconds_accumlator += time_elapsed;
    if (seconds_accumlator >= 1.0) {
      seconds_elapsed += (int)seconds_accumlator;
      seconds_accumlator = 0.0;
    }
    while (*lag >= TICK_RATE) {
      *lag -= TICK_RATE;
    }

    start_timer(&timer, (int)seconds_elapsed);
    print_progress(txt, progress);

    if (*time < (int)seconds_elapsed && i <= text_length) {
      *running = false;
    }
    usleep(12000);
  };
}

int main(int argc, char *argv[]) {
  double last_time = get_time_in_seconds();
  double lag = 0.0;

  int no_of_words = 0;
  int time_in_seconds = 0;
  bool running = true;
  show_start_menu(&no_of_words, &time_in_seconds);
  start_test(&no_of_words, &time_in_seconds, &lag, &running);
  return 0;
}
