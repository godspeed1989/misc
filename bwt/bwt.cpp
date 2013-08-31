
#include "bwt.hpp"
#include <string.h>
#include <stdlib.h>
#include <map>
#include <iostream>
#include <algorithm>

static void sort_dat(u8* dat, const u32 len)
{
	u32 i, j;
	for(i=0;i<len;i++)
		for(j=i+1;j<len;j++)
			if(dat[i] > dat[j])
			{
				u8 ddd = dat[i];
				dat[i] = dat[j];
				dat[j] = ddd;
			}
}

static void sort_dats(u8** dats, const u32 len, const u32 num)
{
	u32 i, j;
	for(i=0;i<num;i++)
		for(j=i+1;j<num;j++)
			if(compare(dats[i], dats[j], len) < 0)
			{
				u8* dat = dats[i];
				dats[i] = dats[j];
				dats[j] = dat;
			}
}

u32 bwt::encode(u8* data, const u32 len, u8* result)
{
	u32 i, final_char_pos;
	u8* block[len];

	// construct suffix array
	for (i = 0; i < len; ++i)
	{
		block[i] = (u8*)malloc(len*sizeof(u8));
		memcpy(block[i], data + i, len - i);
		memcpy(block[i] + (len-i), data, i);
	}

	sort_dats(block, len, len);

	for (i = 0; i < len; ++i)
	{
		result[i] = block[i][len - 1];
		if(compare(block[i], data, len)==0)
			final_char_pos = i;
	}

	for (i = 0; i < len; ++i)
	{
		free(block[i]);
	}
	return final_char_pos;
}

void bwt::decode(u8* data, const u32 len, u32 final_char_pos, u8* result)
{
	u32 i;
	u8 *F;
	u32 *LF;

	F = (u8*)malloc(len*sizeof(u8));
	memcpy(F, data, len);
	sort_dat(F, len);

	// map each char with a unique number
	std::map<u8, u32> C;
	{
		for(i = 0 ; i < len; ++i)
		{
			if (C.count(F[i]) == 0)
			{
				C[F[i]] = i;
			}
			//std::cout << F[i] << ", " << C[F[i]] << std::endl;
		}
	}
	
	// counter occurence of each char
	LF = (u32*)malloc(len*sizeof(u32));;
	{
		std::map<u8, u32> occ;
		for(i = 0 ; i < len; ++i)
		{
			++occ[data[i]];
			LF[i] = C[data[i]] + occ[data[i]] - 1; // -1 means index offset (1,2,3,...) to (0,1,2,...)
			//std::cout << data[i] << ", " << C[data[i]] << ", " << occ[data[i]]<< ", " << LF[i] << std::endl;
		}
	}

	for(i = 0 ; i < len; ++i)
	{
		result[len-i-1] = data[final_char_pos];
		final_char_pos = LF[final_char_pos] ;
	}
	
	free(F);
	free(LF);
}

