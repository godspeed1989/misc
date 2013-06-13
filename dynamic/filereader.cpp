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
		// step 1. read in log's head
		vector<PARA_entity*> &log_head_fmt = xfreader.format_file.log_head;
		ret = readin_entities(dfreader, log_head_fmt, log_data.head);
		//printf("read in [%s] file log (%d)\n", dat_file_name, ret);
		if(ret < 0)
		{
			printf("read in [%s] log head error (%d)\n", dat_file_name, ret);
			break;
		}

		// check log head magic number
		if(2 != get_lenB_by_name(log_data.head, LOG_MAGIC))
			throw;
		u16 magic = *((u16*)get_value_by_name(log_data.head, LOG_MAGIC));
		if(magic != 0xABAB && magic != 0xCDCD)
		{
			printf("log head magic check error %x\n", magic);
			dfreader.info();
			ret = -1;
			break;
		}

		// get log head 'type' attr to determine log's type
		if(2 != get_lenB_by_name(log_data.head, LOG_TYPE))
			throw;
		u16 log_type = *((u16*)get_value_by_name(log_data.head, LOG_TYPE));
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
			ret = -1;
			break;
		}

		// get log head 'length' attr to determine log content length
		if(4 != get_lenB_by_name(log_data.head, LOG_LEN))
			throw;
		u32 log_len = *((u32*)get_value_by_name(log_data.head, LOG_LEN));
		EndianConvert(&log_len, get_lenB_by_name(log_data.head, LOG_LEN));
		// get log content data
		void * ptr = malloc(log_len);
		u32 lenb = dfreader.readB(ptr, log_len);
		bitfile _freader;
		_freader.open("strfile", ptr, (lenb >> 3));
		free(ptr);

		// step 2. read in log's content
		vector<PARA_entity*> &log_content_fmt = xfreader.format_file.log_fmt[i]->entities;
		ret = readin_entities(_freader, log_content_fmt, log_data.content);
		if(ret < 0)
		{
			printf("read in [%s] log content error (%d)\n", dat_file_name, ret);
			break;
		}
		if(!_freader.eof())
		{
			log_data.left.ref = NULL;
			log_data.left.lenb = _freader.capb - _freader.sizeb();
			log_data.left.p = malloc((log_data.left.lenb >> 3) + 1);
			_freader.readb(log_data.left.p, log_data.left.lenb);
		}
		if(!_freader.eof())
			throw;
		_freader.close();

		// step 3. add one log data to the logs
		data_file.logs.push_back(log_data);
	}
	printf("read in [%s] with %d logs\n", dat_file_name, data_file.logs.size());
	return ret;
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

static void dump_dats(bitfile &ofile, const vector<data> dats)
{
	if(ofile.otype != WRITE)
		throw;
	for(size_t i = 0; i < dats.size(); ++i)
		ofile.writeb(dats[i].p, dats[i].lenb);
}

// dump all of log data to file
void filereader::dump_all(const char *file)
{
	bitfile ofile;
	ofile.open(file, WRITE);
	dump_dats(ofile, data_file.head);
	for(size_t i = 0; i < data_file.logs.size(); ++i)
	{
		dump_dats(ofile, data_file.logs[i].head);
		dump_dats(ofile, data_file.logs[i].content);
		ofile.writeb(data_file.logs[i].left.p, data_file.logs[i].left.lenb);
	}
	ofile.writeout();
	printf("dump to the data file [%s]\n", file);
	ofile.close();
}

