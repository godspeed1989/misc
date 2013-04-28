#include <unistd.h>
#include <string.h>
#include <stdio.h>
extern char ** environ;
int main()
{
    while(*environ!=NULL)
    {
        printf("%s\n",*environ);
        environ++;
    }
    return 0;
}

