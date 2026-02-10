#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int get_random_dice() {
  int x;
  // srand(getpid());
  x = rand() % 6 + 1;
  return x;
}

int main() {
  srand(getpid());
  printf("starting the dice game\n");
  fflush(stdout);
  int balance = 1000;
  int round = 0;
  while (balance > 0) {
    round++;
    printf("current balance = %d, round = %d\n", balance, round);
    fflush(stdout);
    printf("Please bet an amount \n");
    fflush(stdout);
    int bet = 0;
    while (1) {
      if (scanf("%d", &bet) != 1) {
        printf("Not an integer\n");
        int c;
        while ((c = getchar() != '\n' && c != EOF)) {
          continue;
        }
      } else {
        break;
      }
    }

    if (bet > balance) {
      printf("betting amount can not be greater then your balance\n");
      round--;
      continue;
    }
    printf("you are currently betting %d amount\n", bet);
    printf("please guess a number between 1 and 6 (press 0 to quite)\n");
    fflush(stdout);
    int guess = -1;

    while (guess < 1 || guess > 6) {
      scanf("%d", &guess);
      if (guess == 0) {
        printf("quitting the game \n");
        return 0;
      }

      printf("your guess can not be less than 1 or greater than 6\n ");
      printf("please try again \n");
    }
    // if (guess < 1 || guess > 6) {
    //   printf("quitting the game as you guessed less than 1 or greater than 6
    //   "); return 0;
    // }
    printf("throwing the dice \n");
    sleep(2);
    int dice = get_random_dice();
    printf("dice roll result = %d\n", dice);

    if (dice != guess) {
      balance = balance - bet;
    } else {
      balance = balance + (bet * 3);
    }
    if (balance <= 0) {
      printf("low balance, quitting the game");
      return 0;
    }
  }
}
