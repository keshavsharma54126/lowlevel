#include <stdio.h>

int main()
{
    /*here we want to convert  celcius to farenheit and all the calculations should be performend in floating points*/
    float celc, farh;
    int low, high, step;

    low = -289.0;
    high = 1000.0;
    step = 20;

    celc = low;
    printf("Celcius to Farenheit table \n");
    while (celc <= high)
    {
        farh = ((celc) * (9.0 / 5.0)) + 32.0;

        printf("%6.2f\t %6.2f \n", celc, farh);
        celc = celc + step;
    }
}