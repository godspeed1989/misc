#include "bwt.hpp"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define    LEN     4096

static u32 length;
static u8  data[LEN];
static u8  encoded[LEN];
static u8  decoded[LEN];

void test()
{
	u32 final_char_pos;
	final_char_pos = bw_encode(data, length, encoded);
	bw_decode(encoded, length, final_char_pos, decoded);
	if(memcmp(decoded, data, length))
		cout << "error\n";
}

int main()
{
	u32 i;
	for(;;)
	{
		srand((unsigned int)time(NULL));
		length = rand() % LEN;
		for(i = 0; i < length; ++i)
			data[i] = rand() % 256;
		test();
	}
	return 0;
}

