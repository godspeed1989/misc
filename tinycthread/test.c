#include "tthread.h"
#include <stdio.h>
#include <assert.h>

mtx_t mutex;

static void lock_print(char* str)
{
    if(str[0] == '\0')
        return;

    mtx_lock(&mutex);

    while (*str != '\0')
    {
        thrd_yield();
        fputc(*str, stdout);
        str++;
    }
    fputc('\n', stdout);

    mtx_unlock(&mutex);
}

static int func1(void* arg)
{
    lock_print((char*)arg);

    thrd_exit(0);
    return -1;
}

static int func2(void* arg)
{
    thrd_detach(thrd_current());
    lock_print((char*)arg);

    thrd_exit(0);
    return -1;
}

int main()
{
    int ret;
    thrd_t p1, p2, p3;
    char str1[8], str2[8], str3[8];

    ret = mtx_init(&mutex, 0);
    assert (ret == thrd_success);

    sprintf(str1, "abcdefg");
    sprintf(str2, "1234567");
    sprintf(str3, "7654321");
    if(thrd_create(&p1, func1, str1) != thrd_success)
    {
        fprintf(stderr, "create thread1 error.\n");
        exit(1);
    }
    if(thrd_create(&p2, func1, str2) != thrd_success)
    {
        fprintf(stderr, "create thread2 error.\n");
        exit(1);
    }
    if(thrd_create(&p3, func2, str3) != thrd_success)
    {
        fprintf(stderr, "create thread3 error.\n");
        exit(1);
    }
    thrd_join(p1, NULL);
    thrd_join(p2, NULL);

    mtx_destroy(&mutex);

    return 0;
}
