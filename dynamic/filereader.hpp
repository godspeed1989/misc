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
	PARA_entity* ref;
	u32 lenb;
	void *p;
}data;

typedef struct log_d
{
	vector<data> head;
	vector<data> content;
}log_d;
// content of a data file
typedef struct file_data
{
	vector<data> head; // file's header
	vector<log_d> logs; // content log's head and it's content
}file_data;

typedef class filereader
{
private:
	char fmt_file[MAX_PATH];
	char dat_file[MAX_PATH];
	xmlreader xfreader;
	bitfile dfreader;
	file_data data_file;
public:
	filereader(const char *fmt_file, const char *dat_file);
	int parse_fmt_file();
	int parse_data_file();
	~filereader()
	{
		xfreader.cleanup();
	}
}filereader;

#endif

