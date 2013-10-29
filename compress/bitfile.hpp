#ifndef __BITFILE_H__
#define __BITFILE_H__

#include <cstdio>
#include <cstring>
#include <cstdlib>

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

enum O_TYPE {READ, WRITE};

typedef struct bitfile
{
	char * name; // stream name
	u8 * data;   // stream data
	u32 capb;    // stream capacity in BITS
	u32 pos_B;   // stream pointer in Byte
	u8 pos_b;    // stream pointer in bit (0~7)

	bitfile()
	{
		data = NULL;
		name = (char*)malloc(1024 * sizeof(char));
		init();
	}
	~bitfile()
	{
		free(name);
		if(data)
			free(data);
	}
	void init()
	{
		strcpy(name, "");
		capb = pos_B = pos_b = 0;
	}
	// Open a bitfile
	u32 open(const char *file, O_TYPE ot)
	{
		if(ot == READ) // read in from a existing file
		{
			FILE * fin = fopen(file, "rb");
			if(fin == NULL)
				return -1;
			clean_and_init();
			strcpy(name, file);
			fseek(fin, 0, SEEK_END);
			u32 filesize = ftell(fin);
			capb = filesize << 3;
			data = (u8*)malloc(filesize);
			if(data == NULL)
				return -1;
			fseek(fin, 0, SEEK_SET);
			fread(data, filesize, 1, fin);
			fclose(fin);
			return 0;
		}
		else if(ot == WRITE)
		{
			clean_and_init();
			strcpy(name, file);
			return 0; // do nothing, wait for first write
		}
		else
			return -1;
	}
	// Open a bitfile from memory
	u32 open(const char *filename, void *dat, u32 nbytes)
	{
		if(filename == NULL || dat == NULL || nbytes == 0)
			return -1;
		clean_and_init();
		strcpy(name, filename);
		capb = nbytes << 3;
		data = (u8*)malloc(nbytes);
		if(data == NULL)
			return -1;
		memcpy(data, dat, nbytes);
		return 0;
	}
	
	/* Read functions */
	/* Read nbytes byte(s), return num of bit(s) actually read. */
	u32 readB(void *ptr, u32 nbytes)
	{
		return readb(ptr, nbytes<<3);
	}
	/* Read nbits bit(s), return num of bit(s) actually read. */
	u32 readb(void *ptr, u32 nbits)
	{
		if(eof())
			return 0;
		if(ftellb() + nbits > capb)// not enough to read
		{
			nbits = capb - ftellb();
			printf("bitfile: %s not enough to read, adjust to %u bits\n", name, nbits);
		}
		u8 byte, tmpb;
		u8 bits = nbits % 8;
		u32 nbytes = nbits >> 3;
		u8 * dst = (u8*)ptr;
		// copy whole byte(s), pos_b will not be changed.
		if(pos_b != 0)
		{
			for(u32 i=0; i<nbytes; i++)
			{
				byte = data[pos_B];
				byte >>= pos_b; //move to lower bound
				pos_B++;
				tmpb = data[pos_B];
				tmpb <<= (8-pos_b); //move upper
				byte |= tmpb;
				dst[i] = byte;
			}
		}
		else
		{
			memcpy(dst, data+pos_B, nbytes);
			pos_B += nbytes;
		}
		if(eof())
			return nbytes<<3;
		// read left bit(s) if we have
		if(bits != 0)
		{
			byte = data[pos_B];
			byte >>= pos_b; //move to lower bound
			if(bits <= 8-pos_b) // enough in [pos_B]
			{
				byte &= (0xFF >> (8-bits)); //just need bits bit(s)
				pos_b = pos_b + bits;
				if(pos_b == 8)
				{
					pos_b = 0;
					pos_B++;
				}
			}
			else // bits>8-pos_b, not enough, need [pos_B] and [pos_B+1]
			{
				pos_B++;
				tmpb = data[pos_B];
				u8 left = bits - (8-pos_b); //left bits
				tmpb &= (0xFF >> (8-left)); //get left bits at lower bound
				tmpb <<= (bits-left); //move left bits upper, or 8-pos_b
				byte |= tmpb;
				pos_b = left;
			}
			dst[nbytes] = byte;
		}
		return nbits;
	}
	/* Write functions */
	/* Write nbytes byte(s), return num of bit(s) actual written. */
	u32 writeB(void *ptr, u32 nbytes)
	{
		return writeb(ptr, nbytes << 3);
	}
	/* Write nbits bit(s), return num of bit(s) actual written. */
	u32 writeb(void *ptr, u32 nbits)
	{
		if(data == NULL)
		{
			const u32 size = 25*1024*1024; // 25MB
			data = (u8*)malloc(size * sizeof(u8));
			memset(data, 0, size * sizeof(u8));
			capb = size << 3;
			pos_B = pos_b = 0;
		}
		if(nbits + ftellb() > capb) //capacity not enough, double the size
		{
			u32 size = 2 * ((capb>>3) + 1);
			u8* dat = (u8*)malloc(size * sizeof(u8));
			memset(dat, 0, size * sizeof(u8));
			memcpy(dat, data, sizeB());
			capb = size << 3;
			free(data);
			data = dat;
		}
		u8 byte;
		u8 bits = nbits % 8;
		u32 nbytes = nbits >> 3;
		u8* src = (u8*)ptr;
		// copy whole byte(s), pos_b will not be changed
		if(pos_b != 0)
		{
			for(u32 i=0; i<nbytes; i++)
			{
				byte = (src[i] << pos_b); //move upper
				data[pos_B] |= byte; //[pos_B]
				pos_B++;
				data[pos_B] = (src[i] >> (8-pos_b)); //[pos_B+1]
			}
		}
		else
		{
			memcpy(data+pos_B, src, nbytes);
			pos_B += nbytes;
		}
		// write left bit(s) if have
		if(bits)
		{
			byte = src[nbytes];
			byte <<= pos_b; //move upper
			data[pos_B] |= byte; //put to [pos_B]
			if(bits <= 8-pos_b) //enough in [pos_B]
			{
				pos_b = pos_b + bits;
				if(pos_b == 8)
				{
					pos_b = 0;
					pos_B++;
				}
			}
			else // bits>8-pos_b, not enough, need [pos_B+1]
			{
				pos_B++;
				u32 left = bits - (8-pos_b); //left bits
				byte = src[nbytes];
				byte >>= (bits-left);//move left bits lower, or 8-pos_b
				data[pos_B] = byte;
				pos_b = left;
			}
		}
		return nbits;
	}
	bool eof()
	{
		return (ftellb() == capb);
	}
	void close()
	{
		clean_and_init();
	}
	void clean_and_init()
	{
		if(data)
			free(data);
		data = NULL;
		init();
	}
	/* tell the read/write position of stream */
	u32 ftellb()
	{
		return pos_B * 8 + pos_b;
	}
	u32 sizeB()
	{
		return pos_B + (pos_b!=0);
	}
	/* write otu file to the disk */
	void write_out()
	{
		FILE* fout = fopen(name, "wb");
		if(fout == NULL)
			return;
		fwrite(data, sizeB(), 1, fout);
		fclose(fout);
	}
	void info()
	{
		printf("bitfile [%s] info: WRITE ", name);
		printf("size=%db pos_B=%d pos_b=%d capb=%dKB\n", ftellb(), pos_B, pos_b, capb>>13);
	}
}bitfile;

#endif

