#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>

#define EVENT_SIZE     ( sizeof(struct inotify_event) )
#define BUF_LEN        ( 1024 * (EVENT_SIZE + 16) )

int main()
{
	int length, i, j;
	int fd;
	int wd;
	static struct inotify_event buffer[128];
	/*create inotify instance*/
	fd = inotify_init();
	if(fd < 0)
	{
		perror("inotify_init()");
		return -1;
	}
	printf("inotify init finished.\n");
	wd = inotify_add_watch(fd, ".", \
			IN_MODIFY | IN_CREATE | IN_DELETE);
	printf("inotify watch is added. Gonna watch pwd.\n");
	
	for(j = 0; j < 10; j++)
	{
		length = read(fd, buffer, BUF_LEN);
		if(length < 0)
		{
			perror("read()");
			return -1;
		}
		i = 0;
		while(i < length)
		{
			struct inotify_event *event = (struct inotify_event *)&buffer[i];
			if(event->len)
			{
				if(event->mask & IN_CREATE)
				{
					if(event->mask & IN_ISDIR)
						printf("Dir %s was created.\n", event->name);
					else
						printf("File %s was created.\n", event->name);
				}
				else if(event->mask & IN_DELETE)
				{
					if(event->mask & IN_ISDIR)
						printf("Dir %s was deleted.\n", event->name);
					else
						printf("File %s was deleted.\n", event->name);
				}
				else if(event->mask & IN_MODIFY)
				{
					if(event->mask & IN_ISDIR)
						printf("Dir %s was modified.\n", event->name);
					else
						printf("File %s was modified.\n", event->name);
				}
			}
			i += EVENT_SIZE + event->len;
		}
	}
	inotify_rm_watch(fd, wd);
	close(fd);
	return 0;
}

