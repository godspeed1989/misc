#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned int    uint;
typedef unsigned char   uchar;
static  uint    CRC32[256];
static  char    init = 0;

static void init_table()
{
    uint     i, j, crc;
    for (i = 0; i < 256; i++)
    {
        crc = i;
        for(j = 0; j < 8; j++)
        {
            if(crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc = crc >> 1;
        }
        CRC32[i] = crc;
    }
}

uint crc32(uchar *buf, int len)
{
    uint ret = 0xFFFFFFFF;
    int   i;
    if( !init )
    {
        init_table();
        init = 1;
    }
    for(i = 0; i < len; i++)
    {
        ret = CRC32[((ret & 0xFF) ^ buf[i])] ^ (ret >> 8);
    }
    ret = ~ret;
    return ret;
}

int main(int argc, char* argv[])
{
    FILE * fin;
    uchar * buf;
    long fsize;
    uint ret;
    fin = fopen(argv[1], "rb");
    /* get size */
    fseek(fin, 0, SEEK_END);
    fsize = ftell(fin);
    /* read all */
    buf = (uchar*)malloc(fsize);
    fseek(fin, 0, SEEK_SET);
    fread(buf, fsize, 1, fin);
    ret = crc32(buf, fsize);
    printf("%s %ld: %X\n", argv[1], fsize, ret);
    return 0;
}
