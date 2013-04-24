#include <stdio.h>
static unsigned long s1[256];
static unsigned long s2[256];

int main(int argc, char *argv[])
{
	int i;
	unsigned char c;
	for(i=0; i<256; i++)
	{
		s1[i] = s2[i] = 0;
	}
	FILE *f1, *f2;
	if(argc < 3)
	{
		printf("Usage: %s file1 file2\n", argv[0]);
		return -1;
	}
	// file 1 statistic
	f1 = fopen(argv[1], "rb");
	if(f1 == NULL)
	{
		printf("%s open error\n", argv[1]);
		return -1;
	}
	while(!feof(f1))
	{
		c = fgetc(f1);
		s1[c]++;
	}
	fclose(f1);
	// file 2 statistic
	f2 = fopen(argv[2], "rb");
	if(f2 == NULL)
	{
		printf("%s open error\n", argv[2]);
		return -1;
	}
	while(!feof(f2))
	{
		c = fgetc(f2);
		s2[c]++;
	}
	fclose(f2);
	// check sum
	for(i=0; i<256; i++)
	{
		if(s1[i] != s2[i])
		{
			printf("%x: %ld - %ld\n", i, s1[i], s2[i]);
			return -1;
		}
	}
	return 0;
}

