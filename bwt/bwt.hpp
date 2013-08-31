#ifndef BURROWS_WHEELER_H
#define BURROWS_WHEELER_H

#include <vector>
using namespace std;

typedef unsigned char u8;
typedef unsigned int  u32;

class bwt
{
	public:
		u32 encode(u8* data, const u32 len, u8* result);
		void decode(u8* data, const u32 len, u32 final_char_pos, u8* result);
};

static int compare(const u8* a, const u8* b, u32 l)
{
	u32 i;
	for(i = 0; i < l; ++i)
	{
		if(a[i] < b[i])
			return 1;
		if(a[i] > b[i])
			return -1;
	}
	return 0;
}

#endif

