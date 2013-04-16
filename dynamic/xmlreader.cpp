#include <cstdio>
#include <vector>
#include <cassert>
#include <cstring>
#include <libxml2/libxml/xmlreader.h>
using namespace std;

typedef unsigned int u32;
#define MLEN        128
#define AREADESC    (const xmlChar*)"AREADESC"
#define FILEHEAD    (const xmlChar*)"FILEHEAD"
#define LOGHEAD     (const xmlChar*)"LOGHEAD"
#define LOGTYPE     (const xmlChar*)"LOGTYPE"
#define LOG         (const xmlChar*)"LOG"
#define PARA        (const xmlChar*)"PARA"
#define PARACHOICE  (const xmlChar*)"PARACHOICE"

struct PARA_entity;
typedef enum length_t {
	BIT, BYTE, OTHER_BIT, OTHER_BYTE
}length_t;
typedef struct length
{
	PARA_entity* e;
	int l;
}length;

typedef enum range_t {
	VALUE, RANGE, ANY
}range_t;
typedef struct range
{
	range_t type;
	u32 low, high;
}range;

typedef enum { T_PARA, T_PARACHOICE } PARA_entity_t;
typedef struct PARA_entity
{
	int depth;
	xmlChar* name;
	PARA_entity_t type;
	struct attr
	{
		int type; // type= in attr
		length_t len_t;
		length len;
		PARA_entity* depend; // nil if not have depend 
		range rng; // only in para choice value = 
	}attr;
}PARA_entity;

typedef struct log_t
{
	range rng;  // <LOG value=$rng>
	vector<PARA_entity*> log;
}log_t;
typedef struct file_format
{
	vector<PARA_entity*> file_head;
	vector<PARA_entity*> log_head;
	vector<log_t*> log_types;
}file_format;

#ifdef LIBXML_READER_ENABLED

vector<PARA_entity*>* processing;
file_format format_file;

PARA_entity* dup_PARA_entity(PARA_entity* entity)
{
	PARA_entity *pentity = (PARA_entity*)malloc(sizeof(PARA_entity));
	memset(pentity, 0, sizeof(PARA_entity));
	memcpy(pentity, entity, sizeof(PARA_entity));
	return pentity;
}

void resolveRng(range& rng, const xmlChar* str)
{
	if(xmlStrlen(str)==0)
		rng.type = ANY;
	else
		rng.low = rng.high = atoi((const char*)str);
	//TODO: range value proccessing
}

void processNode(xmlTextReaderPtr reader)
{
	int i, count, type;
	const xmlChar *name, *s;
	static PARA_entity entity;
	// type
	type = xmlTextReaderNodeType(reader);
	if(type != XML_READER_TYPE_ELEMENT)
		return;
	// depth
	entity.depth = xmlTextReaderDepth(reader);
	// <name attr=""></name>
	name = xmlTextReaderConstName(reader);
	if(name == NULL)
	 	name = BAD_CAST "nil";
	if(xmlStrncasecmp(name, AREADESC, MLEN) == 0)
	{
		printf("*****Processing <AREADESC> ...\n");
		assert(entity.depth == 0);
		return;
	}
	else if(xmlStrncasecmp(name, FILEHEAD, MLEN) == 0)
	{
		printf("*****Processing <FILEHEAD> ...\n");
		assert(entity.depth == 1);
		processing = &format_file.file_head;
		return;
	}
	else if(xmlStrncasecmp(name, LOGHEAD, MLEN) == 0)
	{
		printf("*****Processing  <LOGHEAD> ...\n");
		assert(entity.depth == 1);
		processing = &format_file.log_head;
		return;
	}
	else if(xmlStrncasecmp(name, LOGTYPE, MLEN) == 0)
	{
		printf("*****Processing <LOGTYPE> ...\n");
		assert(entity.depth == 1);
		return;
	}
	else if(xmlStrncasecmp(name, LOG, MLEN) == 0)
	{
		printf("***Processing <LOG value=");
		assert(entity.depth == 2);
		assert(xmlTextReaderAttributeCount(reader) > 0);
		log_t *one_log_type = (log_t*)malloc(sizeof(log_t));
		// setup range
		s = xmlTextReaderGetAttributeNo(reader, 0);
		resolveRng(one_log_type->rng, s);
		processing = &one_log_type->log;
		format_file.log_types.push_back(one_log_type);
		printf("'%s'> ...\n", s);
		return;
	}
	else if(xmlStrncasecmp(name, PARA, MLEN) == 0)// <PARA ...
	{
		assert(xmlTextReaderAttributeCount(reader) > 0);
		count = xmlTextReaderAttributeCount(reader);
		for(i=0; i<count; i++)
		{
			s = xmlTextReaderGetAttributeNo(reader, i);
			if(i == 0)//name
				entity.name = xmlStrdup(s);
			else if(i == 1)//type
				entity.attr.type = atoi((const char*)s);
			else if(i == 2)//length ?? depend
				entity.attr.len.l = atoi((const char*)s);
			else if(i == 3)//??
				;
			else
				;
		}
		entity.type = T_PARA;
		processing->push_back(dup_PARA_entity(&entity));
	}
	else if(xmlStrncasecmp(name, PARACHOICE, MLEN) == 0)//<PARACHOCE ...
	{
		entity.type = T_PARACHOICE;
		if(xmlTextReaderAttributeCount(reader) > 0)
		{
			// vlaue = a range
			s = xmlTextReaderGetAttributeNo(reader, 0);
			resolveRng(entity.attr.rng, s);
			vector<PARA_entity*>::reverse_iterator rit;
			for(rit=processing->rbegin(); rit!=processing->rend(); rit++)
				if((*rit)->depth == entity.depth-1) {
					entity.attr.depend = *rit;
					break;
				}
			processing->push_back(dup_PARA_entity(&entity));
		}
		else
		{
			//a PARACHOICE without value= is useless
			processing->pop_back();
			return;
		}
	}
	else
	{
		printf("Unknow XML node type [%s]\n", name);
		exit(-1);
	}
	
	// <name attr0="" attr1=""/>
	if(type != XML_READER_TYPE_END_ELEMENT &&
		xmlTextReaderAttributeCount(reader) > 0)
	{
		printf("[");
		count = xmlTextReaderAttributeCount(reader);
		for(i=0; i<count; i++)
		{
			s = xmlTextReaderGetAttributeNo(reader, i);
			printf(" attr%d='%s' ", i, s);
		}
		printf("]\n");
	}
}

void streamFile(const char* file)
{
	int ret;
	xmlTextReaderPtr reader;
	reader = xmlReaderForFile(file, NULL, XML_PARSE_DTDATTR | XML_PARSE_RECOVER);
	if(reader != NULL)
	{
		processing = NULL;
		ret = xmlTextReaderRead(reader);
		while (ret == 1)
		{
			processNode(reader);
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
		if(ret != 0)
		{
			fprintf(stderr, "Error in parse %s\n", file);
		}
	}
	else
	{
		fprintf(stderr, "Open %s error\n", file);
	}
}

void show_PARA_entity(PARA_entity *entity)
{
	printf("depth=%d ", entity->depth);
	if(entity->type == T_PARA)
		printf("name=%s type=%d length=%d\n",
				entity->name, entity->attr.type, entity->attr.len.l);
	else if(entity->type == T_PARACHOICE)
		printf("CHOICE on %s value=%d~%d\n",
				entity->attr.depend->name, entity->attr.rng.low, entity->attr.rng.high);
}

void show_one_log_type(log_t *log)
{
	vector<PARA_entity*>::iterator it;
	printf("<LOG type=%d~%d >(%d)\n", log->rng.low, log->rng.high, log->log.size());
	for(it = log->log.begin(); it != log->log.end(); it++)
	{
		show_PARA_entity(*it);
	}
}

void output()
{
	vector<PARA_entity*>::iterator it;
	vector<log_t*>::iterator lit;
	printf("******** Output read in to check ********\n");
	printf("-----File head info(%d)-----\n", format_file.file_head.size());
	it = format_file.file_head.begin();
	for(;it != format_file.file_head.end();it++)
		show_PARA_entity(*it);
	printf("-----Log head info(%d)-----\n", format_file.log_head.size());
	it = format_file.log_head.begin();
	for(;it != format_file.log_head.end();it++)
		show_PARA_entity(*it);
	printf("-----Log type info(%d)-----\n", format_file.log_types.size());
	lit = format_file.log_types.begin();
	for(;lit != format_file.log_types.end();lit++)
		show_one_log_type(*lit);
}

int main(int argc, char* argv[])
{
	if(argc < 2)
		return -1;
	LIBXML_TEST_VERSION
	
	streamFile(argv[1]);
	// cleanup function for the XML library
	xmlCleanupParser();
	
	output();
	return 0;
}

#endif

