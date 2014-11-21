/*
 * ����zipf�ֲ���������ʷֲ�
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

static void generate_pf(double *pf, int R, double A)
{
    int i;
    double sum = 0.0;
    const double C = 1.0;   // һ��ȡ1
    for (i = 0; i < R; i++)
    {
        sum += C/pow((double)(i+2), A);
    }
    for (i = 0; i < R; i++)
    {
        if (i == 0)
            pf[i] = C/pow((double)(i+2), A)/sum;
        else
            pf[i] = pf[i-1] + C/pow((double)(i+2), A)/sum;
    }
}

/**
 * num  : �������
 * A    : �ֲ���ǰ�벿�ֵİٷֱȣ�0<A<1
 * Range: ���ʷֲ���Χ
 * out  : ��������out[i] = [0, Range)
 */
void zipf_pick(int num, int Range, double A, int *out)
{
    int i;
    double *pf; // ֵΪ0~1֮��, �ǵ���f(r)���ۼ�ֵ

    A = 1.0 - A;
    pf = (double*)malloc(Range*sizeof(double));
    generate_pf (pf, Range, A);

    srand((unsigned int)time(NULL));
    for (i = 0; i < num; i++)
    {
        int index = 0;
        double data = (double)rand()/RAND_MAX;
        while (data > pf[index])
            index++;
        out[i] = index;
    }
    free(pf);
}

#define NUM     1000
#define RANGE   100

int main()
{
    int i, out[NUM], stat[RANGE];

    zipf_pick(NUM, RANGE, 0.25, out);

    memset(stat, 0, sizeof(int)*RANGE);
    for (i = 0; i < NUM; i++)
        stat[out[i]]++;
    for (i = 0; i < RANGE; i++)
        printf("%d\t%d\n", i, stat[i]);

    return 0;
}
