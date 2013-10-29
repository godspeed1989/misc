#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bitfile.hpp"

// Arithmetic coding sample

#define ENCODE 0
#define DECODE 1

typedef struct cnt
{
	u32 count[2];
}cnt;


u32 x1; // lower bound
u32 x2; // upper bound
#define PREDICT_RNG 4096
u32 p;  // predict 0~4095
cnt *context;
#define BUF_BITS  16
u32 buff; // nearest BUF_BITS
u32 state;
u32 x;

// store current bit
// return the prediction of 0 for the next bit
void predictor(u32 y)
{
	context[buff].count[y]++;
	buff <<= 1;
	buff |= y;
	buff &= ((1<<BUF_BITS) - 1);
	// '0' / ('0' + '1')
	float r = context[buff].count[0] / (context[buff].count[0] + context[buff].count[1]);
	p = (u32)(PREDICT_RNG * r);
}

void code(bitfile* fin, bitfile* fout)
{
	// get a bit
	u32 y;
	if(state == ENCODE)
	{
		y = 0;
		fin->readb(&y, 1);
	}
	u32 rng = x2 - x1;
	// rng * (p / 4096) = (rng / 4096) * p + (rng & fff) * p / 4096
	u32 xmid = x1 + ((rng>>12)*p) + (((rng&0xfff)*p)>>12);
	if(state == DECODE)
	{
		y = (x > xmid) ? 0 : 1;
		fout->writeb(&y, 1);
	}
	// 1: lower, 0: upper
	y ? (x2 = xmid) : (x1 = xmid+1);

	predictor(y);

	// pass *equal* leading bytes of range 
	while (((x1^x2)&0xff000000)==0)
	{
		u8 c = 0;
		if(state == ENCODE)
		{
			c = (x2 >> 24);
			fout->writeB(&c, 1);
		}
		else
		{
			fin->readB(&c, 1);
			x <<= 8;
			x |= c;
		}
		// adjust range
		x2 = (x2<<8) + 255;
		x1 = (x1<<8);
	}
}


void init()
{
	x1 = 0x00000000;
	x2 = 0xFFFFFFFF;
	p = PREDICT_RNG / 2;
	buff = 0;
	context = (cnt*)malloc(sizeof(cnt) * (1<<BUF_BITS));
	for(u32 i = 0; i < (1<<BUF_BITS); i++)
	{
		context[i].count[0] = context[i].count[1] = 1;
	}
}

int main(int argc, const char* argv[])
{
	bitfile fin, fout;
	if(argc < 4)
	{
		printf("Usage: e/d  input  output\n");
		return 1;
	}
	if(argv[1][0] == 'e') state = ENCODE;
	if(argv[1][0] == 'd') state = DECODE;
	fin.open(argv[2], READ);
	fout.open(argv[3], WRITE);

	init();
	u8 *p;
	u32 i, size;
	if(state == ENCODE)
	{
		p = (u8*)&x2;
		size = fin.capb;
		size >>= 3;
		fout.writeB(&size, 4);
		while(!fin.eof())
			code(&fin, &fout);
		for(i=0; i<4; i++)
			fout.writeB(p+3-i, 4); // why upper bound
	}
	else
	{
		p = (u8*)&x;
		fin.readB(&size, 4);
		for(i=0; i<4; i++)
			fin.readB(p+3-i, 1);
		size <<= 3;
		while(size--)
			code(&fin, &fout);
	}

	fout.write_out();
	fin.close();
	fout.close();
	return 0;
}
