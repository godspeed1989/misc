#include "filereader.hpp"
#include "filereader_static.hpp"
#include "endian.hpp"
#include <cstring>

#define LOG_MAGIC   (const xmlChar*)"Verifyflag"
#define LOG_TYPE    (const xmlChar*)"Logtype"
#define LOG_LEN     (const xmlChar*)"Loglength"

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
	bitfile dfreader;
#if 1
	xfreader.printOut();
#endif
	if(dfreader.open(dat_file_name, READ))
	{
		printf("Open %s error\n", dat_file_name);
		return -1;
	}
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
		vector<PARA_entity*> &log_head_fmt = xfreader.format_file.log_head;
		ret = readin_entities(dfreader, log_head_fmt, log_data.head);
		printf("read in [%s] file log (%d)\n", dat_file_name, ret);
		if(ret < 0)
		{
			printf("read in [%s] log head error (%d)\n", dat_file_name, ret);
			return ret;
		}

		// check log head magic number
		u16 magic = *((u16*)get_value_by_name(log_data.head, LOG_MAGIC));
		if(magic != 0xABAB && magic != 0xCDCD)
		{
			printf("log head magic check error %x\n", magic);
			dfreader.info();
			return 1;
		}

		// get log head 'type' attr to determine log's type
		u32 log_type = *((u32*)get_value_by_name(log_data.head, LOG_TYPE));
		EndianConvert(&log_type, get_lenB_by_name(log_data.head, LOG_TYPE));
		// get log content format by the value of log's type
		for(i = 0; i < xfreader.format_file.log_fmt.size(); ++i)
		{
			if(range_equal(xfreader.format_file.log_fmt[i]->rng, log_type))
				break;
		}
		if(i == xfreader.format_file.log_fmt.size())
		{
			printf("unkown log type %d\n", log_type);
			return -1;
		}

		// get log head 'length' attr to determine log content length
		u32 log_len = *((u32*)get_value_by_name(log_data.head, LOG_LEN));
		EndianConvert(&log_len, get_lenB_by_name(log_data.head, LOG_LEN));
		// get log content data
		void * ptr = malloc(log_len);
		dfreader.readB(ptr, log_len);
		bitfile _freader;
		_freader.open("strfile", ptr, log_len);
		_freader.info();
		free(ptr);

		// read in log's content
		vector<PARA_entity*> &log_content_fmt = xfreader.format_file.log_fmt[i]->entities;
		ret = readin_entities(_freader, log_content_fmt, log_data.content);
		if(ret < 0)
		{
			printf("read in [%s] log content error (%d)\n", dat_file_name, ret);
			return ret;
		}
		_freader.close();

		// add one log data to the logs
		data_file.logs.push_back(log_data);
	}
	printf("read in [%s] with %d logs\n", dat_file_name, data_file.logs.size());
	return 0;
}

// make a summay about parse result
void filereader::summary()
{
	printf("~~~summary~~~\n");
	printf("%s %s\n", fmt_file_name, dat_file_name);
	printf("head %d\n", data_file.head.size());
	printf("logs %d\n", data_file.logs.size());
	printf("~~~~~~~~~~~~~\n");
}

