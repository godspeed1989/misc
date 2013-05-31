#ifndef __FILEREADER_HPP__
#define __FILEHEADER_HPP__

#include "xmlreader.hpp"
#include "../bit_file/bitfile.hpp"
#include <vector>
using namespace std;

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

typedef struct data
{
	u32 pos_B;  // file pointer in Bytes
	u8 pos_b;   // file pointer in bits
	PARA_entity* ref;
}data;

typedef struct log
{
	vector<data> head;
	vector<data> content;
}log;

typedef struct file_data
{
	vector<data> head;
	vector<log> logs;
}file_data;

typedef class filereader
{
private:
	char fmt_file[MAX_PATH];
	char dat_file[MAX_PATH];
	xmlreader xfreader;
	bitfile bit_reader;
	file_data data_file;
public:
	filereader(const char *fmt_file, const char *dat_file);
	int parse_fmt_file()
	{
		return xfreader.processFile(fmt_file);
	}
	int parse_data_file();
	~filereader()
	{
		xfreader.cleanup();
	}
}filereader;

#endif

