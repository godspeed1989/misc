#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>

int nic_is_running(const char *nic_name)
{
	struct ifreq ifr;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd < 0)
		return -1;

	strcpy(ifr.ifr_name, nic_name);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
	{
		close(skfd);
		return -1;
	}
	close(skfd);
	return (ifr.ifr_flags & IFF_RUNNING)? 0 : 1;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Usage: ./a.out nic_name\n");
		return 0;
	}
	if(nic_is_running(argv[1]) == 0)
		printf("%s is running\n", argv[1]);
	else
		printf("%s is not running\n", argv[1]);
	return 0;
}

