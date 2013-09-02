#ifndef BURROWS_WHEELER_H
#define BURROWS_WHEELER_H

typedef unsigned char u8;
typedef unsigned int  u32;

u32 bw_encode(u8* data, const u32 len, u8* result);

void bw_decode(u8* data, const u32 len, u32 final_char_pos, u8* result);

#endif

