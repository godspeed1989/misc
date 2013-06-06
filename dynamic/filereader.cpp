#include "filereader.hpp"
#include "filereader_static.hpp"
#include <cstring>

#define LOGTYPE    (const xmlChar*)"Logtype"

filereader::filereader(const char *fmtfile, const char *datfile)
{
	strncpy(fmt_file_name, fmtfile, MAX_PATH);
	strncpy(dat_file_name, datfile, MAX_PATH);
}

// parse and read in XML format file
int filereader::parse_fmt_file()
{
	return xfreader.processFile(fmt_file_name);
}

// parse and read in data file
int filereader::parse_data_file()
{
	int ret;
	size_t i;
	xfreader.printOut();
	if(dfreader.open(dat_file_name, READ))
	{
		printf("Open %s error\n", dat_file_name);
		return -1;
	}
	dfreader.info();
	// read in data file's header
	vector<PARA_entity*> &file_head_fmt = xfreader.format_file.file_head;
	ret = readin_entities(dfreader, file_head_fmt, data_file.head);
	printf("read in [%s] file head (%d)\n", dat_file_name, ret);
	if(ret < 0)
		return ret;
	// read in data file's logs
	while(!dfreader.eof())
	{
		log_data log_data;
		// read in log's head
		vector<PARA_entity*> &log_head_fmt = xfreader.format_file.file_head;
		ret = readin_entities(dfreader, log_head_fmt, log_data.head);
		if(ret < 0)
		{
			printf("read in [%s] log head error (%d)\n", dat_file_name, ret);
			return ret;
		}

		// get log head data 'type' attr to determine log's type
		long log_type = get_value_by_name(log_data.head, LOGTYPE);
		// get log content format by the value of log's type
		for(i = 0; i < xfreader.format_file.log_fmt.size(); ++i)
		{
			if(range_equal(xfreader.format_file.log_fmt[i]->rng, log_type))
				break;
		}
		if(i == xfreader.format_file.log_fmt.size())
		{
			printf("unkown log type %ld\n", log_type);
			return -1;
		}

		// read in log's content
		vector<PARA_entity*> &log_content_fmt = xfreader.format_file.log_fmt[i]->entities;
		ret = readin_entities(dfreader, log_content_fmt, log_data.content);
		if(ret < 0)
		{
			printf("read in [%s] log content error (%d)\n", dat_file_name, ret);
			return ret;
		}

		// add one log data to the logs
		data_file.logs.push_back(log_data);
	}
	printf("read in [%s] with %d logs\n", dat_file_name, data_file.logs.size());
	return 0;
}

