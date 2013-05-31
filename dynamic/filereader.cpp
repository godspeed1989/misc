#include "filereader.hpp"
#include <cstring>

filereader::filereader(const char *fmtfile, const char *datfile)
{
	strncpy(fmt_file, fmtfile, MAX_PATH);
	strncpy(dat_file, datfile, MAX_PATH);
}

int filereader::parse_data_file()
{
	size_t i;
	static data dat;
	xfreader.printOut();
	if(bit_reader.open(dat_file, READ))
	{
		printf("Open %s error\n", dat_file);
		return -1;
	}
	bit_reader.info();
	// file header
	for(i = 0; i<xfreader.format_file.file_head.size(); ++i)
	{
		dat.pos_B = bit_reader.pos_B;
		dat.pos_b = bit_reader.pos_b;
		dat.ref = xfreader.format_file.file_head[i];
		data_file.head.push_back(dat);
		//TODO
	}
	//TODO
	return 0;
}

