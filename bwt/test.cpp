#include "bwt.hpp"
#include <string.h>
#include <iostream>
using namespace std;

#define    LEN     128 
static u8 data[LEN] = "banana";
static u8 encoded[LEN] = "";
static u8 decoded[LEN] = "";

void test()
{
	bwt b;
	u32 final_char_pos;
	final_char_pos = b.encode(data, 6, encoded);
	b.decode(encoded, 6, final_char_pos, decoded);
	if(compare(decoded, data, 6))
		cout << "error\n";
}

int main()
{
	test();

	return 0;
}
