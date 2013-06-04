#include "filereader.hpp"
#include <cstring>

#define T_BIT          3
#define T_BYTE         4
#define T_BIT_OTHER    7
#define T_BYTE_OTHER   16

filereader::filereader(const char *fmtfile, const char *datfile)
{
	strncpy(fmt_file, fmtfile, MAX_PATH);
	strncpy(dat_file, datfile, MAX_PATH);
}

int filereader::parse_fmt_file()
{
	return xfreader.processFile(fmt_file);
}

static int readin(bitfile &reader, PARA_entity* e, vector<data> &container)
{
	data d;
	switch(e->attr.type)
	{
		case T_BIT:
			d.lenb = e->attr.len.l;      break;
		case T_BYTE:
			d.lenb = e->attr.len.l << 3; break;
		case T_BIT_OTHER:
			//TODO len is based other in BIT
			break;
		case T_BYTE_OTHER:
			//TODO len is based other in BYTE
			break;
		default:
			d.lenb = e->attr.len.l << 3;
			break;
	}
	d.ref = e;
	d.p = malloc((d.lenb >> 3) + 1);
	reader.readb(d.p, d.lenb);
	if(reader.eof())
		return -1;
	container.push_back(d);
	return 0;
}

int filereader::parse_data_file()
{
	size_t i;
	log_d log;
	xfreader.printOut();
	if(dfreader.open(dat_file, READ))
	{
		printf("Open %s error\n", dat_file);
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
	// read in data file's logs
	vector<PARA_entity*> &log_head = xfreader.format_file.file_head;
	while(!dfreader.eof())
	{
		log.head.clear();
		log.content.clear();
		// read in log's head
		for(i = 0; i<log_head.size(); ++i)
		{
			if(readin(dfreader, log_head[i], log.head))
			{
				printf("Read log head error\n");
				return -1;
			}
		}
		// TODO get log.head.type to determine log_types
		vector<PARA_entity*> &log_types = xfreader.format_file.log_types[2222]->logs;
		// read in log's content by base on the log type
		for(i = 0; i<log_types.size(); ++i)
		{
			if(readin(dfreader, log_types[i], log.content))
			{
				printf("Read log content error\n");
				return -1;
			}
		}
		data_file.logs.push_back(log);
	}
	
	return 0;
}

