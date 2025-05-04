#include <stdio.h>

#define LOWER 0
#define HIGHER 100
#define STEP 20

int main()
{

    for (float lower = LOWER; lower <= HIGHER; lower += 20)
    {
        printf("%3.1f \t %3.1f \n", lower, (5.0 / 9.0) * (lower - 32));
    }
}