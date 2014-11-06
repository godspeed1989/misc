/**
 * 生成指定个数的1和0的所有排列
 */
#include <stdio.h>
#include <malloc.h>
#define NUMBER_ONE      3
#define NUMBER_ZERO     3
#define NUMBER_TOTAL    (NUMBER_ZERO+NUMBER_ONE)

int result[NUMBER_TOTAL];

void result_out()
{
    int i;
    for(i = 0; i < NUMBER_TOTAL; i++)
        printf("%d ", result[i]);
    printf("\n");
}

int main()
{
    int i, x, y;

    for(i = 0; i < NUMBER_ONE; i++)
        result[i] = 1;
    for(; i < NUMBER_TOTAL; i++)
        result[i] = 0;

    result_out();
    x = y = NUMBER_ONE - 1;

    while (x < (NUMBER_TOTAL-1))
    {
        result[x] = 0;
        result[y] = 1;
        result[0] = result[x+1];
        result[x+1] = 1;
        x = 1 + (x)*(1-(result[1])*(1-result[0]));
        y = result[0]*(y+1);
        result_out();
    }
    return 0;
}
