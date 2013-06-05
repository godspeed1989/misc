#include "filereader.hpp"
#include "filereader_static.hpp"
#include <cstring>

#define LOGTYPE    (const xmlChar*)"Logtype"

filereader::filereader(const char *fmtfile, const char *datfile)
{
	strncpy(fmt_file_name, fmtfile, MAX_PATH);
	strncpy(dat_file_name, datfile, MAX_PATH);
}

int filereader::parse_fmt_file()
{
	return xfreader.processFile(fmt_file_name);
}

int filereader::parse_data_file()
{
	size_t i;
	log_data logdata;
	xfreader.printOut();
	if(dfreader.open(dat_file_name, READ))
	{
		printf("Open %s error\n", dat_file_name);
		return -1;
	}
	dfreader.info();
	// read in data file's header
	vector<PARA_entity*> &file_head = xfreader.format_file.file_head;
	for(i = 0; i<file_head.size(); ++i)
	{
		if(readin(dfreader, file_head[i], data_file.head))
		{
			printf("Read file head error\n");
			return -1;
		}
	}
	printf("read in [%s] file header\n", dat_file_name);
	// read in data file's logs
	vector<PARA_entity*> &log_head = xfreader.format_file.file_head;
	while(!dfreader.eof())
	{
		logdata.head.clear();
		logdata.content.clear();
		// read in log's head
		for(i = 0; i<log_head.size(); ++i)
		{
			if(readin(dfreader, log_head[i], logdata.head))
			{
				printf("Read log head error\n");
				return -1;
			}
		}
		// TODO get log head's type attr to determine log's type
		long ltype = get_value_by_name(logdata.head, LOGTYPE);
		// TODO get by value
		for(i = 0; i < xfreader.format_file.log_fmt.size(); ++i)
		{
			if(range_equal(xfreader.format_file.log_fmt[i]->rng, ltype))
				break;
		}
		if(i == xfreader.format_file.log_fmt.size())
		{
			printf("Log head type error %lx\n", ltype);
			return -1;
		}	
		vector<PARA_entity*> &log_content = xfreader.format_file.log_fmt[ltype]->entitys;
		// read in log's content
		for(i = 0; i<log_content.size(); ++i)
		{
			if(readin(dfreader, log_content[i], logdata.content))
			{
				printf("Read log content error\n");
				return -1;
			}
		}
		data_file.logs.push_back(logdata);
	}
	printf("read in [%s] file %d logs\n", dat_file_name, data_file.logs.size());
	return 0;
}

