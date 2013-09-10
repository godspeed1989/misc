
#include "bwt.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

typedef struct block_elem
{
	const u8 * ptr;
	u32 len, idx;
	void setup(const u8 *p, const u32 l, const u32 i)
	{
		ptr = p;
		len = l;
		idx = i;
	}
	u8 at(u32 i) const
	{
		return ptr[(idx + i) % len];
	}
}block_elem;

struct elemcmp : public std::binary_function<block_elem*, block_elem*, bool>
{
	bool operator ()( const block_elem *a, const block_elem *b ) const
	{
		u32 i;
		if(a->len != b->len)
			throw;
		for(i = 0; i < a->len; ++i)
		{
			if(a->at(i) < b->at(i))
				return true;
			else if(a->at(i) > b->at(i))
				return false;
		}
		return false;
	}
};

u32 bw_encode(u8* data, const u32 len, u8* result)
{
	u32 i, final_char_pos;

	// construct suffix array
	block_elem *be;
	std::vector<block_elem*> block_elems;
	for (i = 0; i < len; ++i)
	{
		be = (block_elem*)malloc(sizeof(block_elem));
		be->setup(data, len, i);
		block_elems.push_back(be);
	}

	elemcmp cmp;
	std::sort(block_elems.begin(), block_elems.end(), cmp);

	for (i = 0; i < len; ++i)
	{
		result[i] = block_elems[i]->at(len - 1);
		if(block_elems[i]->idx == 0)
			final_char_pos = i;
	}

	for (i = 0; i < len; ++i)
	{
		free(block_elems[i]);
	}
	return final_char_pos;
}

void bw_decode(u8* data, const u32 len, u32 final_char_pos, u8* result)
{
	u32 i;
	u8 *F;
	u32 *LF;

	F = (u8*)malloc(len * sizeof(u8));
	memcpy(F, data, len);
	std::sort(F, F + len);

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

