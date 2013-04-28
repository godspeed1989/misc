#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX_LEN 100

void input_handler(int num)
{
	char data[MAX_LEN];
	int len;
	len = read(STDIN_FILENO, &data, MAX_LEN);
	data[len] = 0;
	printf("Input is: %s\n", data);
}

int main()
{
	int oflags;
	signal(SIGIO, input_handler);
	// set owner
	fcntl(STDIN_FILENO, F_SETOWN, getpid());
	// set flag
	oflags = fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO, F_SETFL, oflags | FASYNC);
	while(1);
	return 0;
}

