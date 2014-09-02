#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SIDE 500
#define NUM_PARTICLES 5000

bool W[SIDE][SIDE];

int main()
{
    int i, r, c;
    srand((unsigned)time(NULL));
    W[SIDE / 2][SIDE / 2] = true;

    for (i = 0; i < NUM_PARTICLES; i++)
    {
        unsigned int x, y;
        OVER:
        /* find a blank pixel */
        do {
            x = rand() % (SIDE - 2) + 1;
            y = rand() % (SIDE - 2) + 1;
        } while (W[y][x]);

        /* while surroundings are all blank */
        while (W[y-1][x-1] + W[y-1][x] + W[y-1][x+1] +
               W[y][x-1]               + W[y][x+1] +
               W[y+1][x-1] + W[y+1][x] + W[y+1][x+1] == 0)
        {
            unsigned int dxy = rand() % 8;
            if (dxy > 3) dxy++;
            x += (dxy % 3) - 1;
            y += (dxy / 3) - 1;
            if (x < 1 || x >= SIDE - 1 || y < 1 || y >= SIDE - 1)
                goto OVER;
        }

        W[y][x] = true;
    }

    printf("P1\n%d %d\n", SIDE, SIDE);
    for (r = 0; r < SIDE; r++)
    {
        for (c = 0; c < SIDE; c++)
            printf("%d ", W[r][c]);
        putchar('\n');
    }
    return 0;
}
