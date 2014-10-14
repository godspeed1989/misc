/*
  Demonstrate the use of setjmp() and longjmp() to perform a nonlocal goto.

  The presence of a command-line argument determines which of two functions (f1() or f2()) we will longjmp() from.
 */
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>

static jmp_buf env;

void f2(void)
{
    longjmp(env, 2);
}

void f1(int argc)
{
    if (argc == 1)
        longjmp(env, 1);
    f2();
}

int main(int argc, char *argv[])
{
    switch(setjmp(env)) {
    case 0:
        printf("Initial setjmp()\n");
        printf("Call f1()\n");
        f1(argc);
        break;
    case 1:
        printf("Jumped back from f1()\n");
        break;
    case 2:
        printf("Jumped back from f2()\n");
        break;
    }
    exit(EXIT_SUCCESS);
}
