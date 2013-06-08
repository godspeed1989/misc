#include "endian.h"
#include <stdio.h>

void EndianConvert(void *_value, int nbytes)
{
	unsigned char temp;
	unsigned long *value = (unsigned long *)_value;
	switch (nbytes)
	{
		case 2:
			temp = (*value & 0x00ff) >> 0;
			*value &= 0xff00;
			*value |= *value >> 8;
			*value &= 0x00ff;
			*value |= temp << 8;
			break;
		case 4:
			temp = (*value & 0x000000ff) >> 0;
			*value &= 0xffffff00;
			*value |= (*value & 0xff000000) >> 24;
			*value &= 0x00ffffff;
			*value |= temp << 24;

			temp = (*value & 0x0000ff00) >> 8;
			*value &= 0xffff00ff;
			*value |= (*value & 0x00ff0000) >> 8;
			*value &= 0xff00ffff;
			*value |= temp << 16;
			break;
		default:
			printf("not supported endian conv len %d\n", nbytes);
			break;
	}
}

