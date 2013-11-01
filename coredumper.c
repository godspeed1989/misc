/**
 * google coredumper example
 * compile: gcc coredumper.c -lcoredumper -o dumper
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <google/coredumper.h>

jmp_buf env;
void sig_func(int signo)
{
    char file[32];
    printf("segment falut: caught signal: %d\n", signo);
    sprintf(file, "coredump_%d.core", signo);
    if( WriteCoreDump(file) == 0 )
        printf("success: WriteCoreDump to %s\n", file);
    else
        printf("failure: WriteCoreDump to %s\n", file);
    longjmp(env, 1);
}

void error1()
{
    int a = 5/0;
}

void error2()
{
    char *q = 0;
    q[1] = 's';
}

int main()
{
    signal(SIGSEGV, &sig_func);
    signal(SIGFPE,  &sig_func);
    if(setjmp(env) == 0)
        error1();
    else
        fprintf(stderr, "dumped\n");
    if(setjmp(env) == 0)
        error2();
    else
        fprintf(stderr, "dumped\n");
    return 0;
}

