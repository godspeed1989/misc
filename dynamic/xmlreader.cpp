#include "xmlreader.hpp"
#include <cstdio>
#include <cassert>
#include <cstring>

#define MLEN        128
#define AREADESC    (const xmlChar*)"AREADESC"
#define FILEHEAD    (const xmlChar*)"FILEHEAD"
#define LOGHEAD     (const xmlChar*)"LOGHEAD"
#define LOGTYPE     (const xmlChar*)"LOGTYPE"
#define LOG         (const xmlChar*)"LOG"
#define PARA        (const xmlChar*)"PARA"
#define PARACHOICE  (const xmlChar*)"PARACHOICE"

#ifdef LIBXML_READER_ENABLED

static PARA_entity* dup_PARA_entity(PARA_entity* entity)
{
	PARA_entity *pentity = (PARA_entity*)malloc(sizeof(PARA_entity));
	memset(pentity, 0, sizeof(PARA_entity));
	memcpy(pentity, entity, sizeof(PARA_entity));
	return pentity;
}

static void resolveRng(range& rng, const xmlChar* str)
{
	if(xmlStrlen(str)==0)
		rng.type = ANY;
	else
		rng.low = rng.high = atoi((const char*)str);
	//TODO: range value proccessing
}

/**
 * process one XML node
 */
void xmlreader::processNode(xmlTextReaderPtr reader)
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
	if(xmlStrncasecmp(name, AREADESC, MLEN) == 0)// <AREADESC>
	{
		printf("*****Processing <AREADESC> ...\n");
		assert(entity.depth == 0);
		return;
	}
	else if(xmlStrncasecmp(name, FILEHEAD, MLEN) == 0)// <FILEHEAD>
	{
		printf("*****Processing <FILEHEAD> ...\n");
		assert(entity.depth == 1);
		processing = &format_file.file_head;
		return;
	}
	else if(xmlStrncasecmp(name, LOGHEAD, MLEN) == 0)// <LOGHEAD>
	{
		printf("*****Processing  <LOGHEAD> ...\n");
		assert(entity.depth == 1);
		processing = &format_file.log_head;
		return;
	}
	else if(xmlStrncasecmp(name, LOGTYPE, MLEN) == 0)// <LOGTYPE>
	{
		printf("*****Processing <LOGTYPE> ...\n");
		assert(entity.depth == 1);
		return;
	}
	else if(xmlStrncasecmp(name, LOG, MLEN) == 0)// <LOG type="" ...
	{
		printf("***Processing <LOG value=");
		assert(entity.depth == 2);
		assert(xmlTextReaderAttributeCount(reader) > 0);
		log_t *one_log_type = new log_t;
		// setup range
		s = xmlTextReaderGetAttributeNo(reader, 0);
		resolveRng(one_log_type->rng, s);
		processing = &one_log_type->logs;
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
	
#ifdef DEBUG
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
#endif
}

void xmlreader::processFile(const char* file)
{
	int ret;
	xmlTextReaderPtr reader;
	reader = xmlReaderForFile(file, NULL, XML_PARSE_DTDATTR | XML_PARSE_RECOVER);
	if(reader != NULL)
	{
		processing = NULL;
		cleanup();
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

static void show_PARA_entity(PARA_entity *entity)
{
	printf("depth=%d ", entity->depth);
	if(entity->type == T_PARA)
		printf("name=%s type=%d length=%d\n",
				entity->name, entity->attr.type, entity->attr.len.l);
	else if(entity->type == T_PARACHOICE)
		printf("CHOICE on %s value=%d~%d\n",
				entity->attr.depend->name, entity->attr.rng.low, entity->attr.rng.high);
}

static void show_one_log_type(log_t *log)
{
	printf("<LOG type=%d~%d >(%d)\n", log->rng.low, log->rng.high, log->logs.size());
	vector<PARA_entity*>::iterator it;
	for(it = log->logs.begin(); it != log->logs.end(); it++)
	{
		show_PARA_entity(*it);
	}
}

void xmlreader::printOut()
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

void xmlreader::cleanup()
{
	format_file.file_head.clear();
	format_file.log_head.clear();
	vector<log_t*>::iterator lit = format_file.log_types.begin();
	while(lit != format_file.log_types.end())
	{
		vector<PARA_entity*>::iterator pit = (*lit)->logs.begin();
		while(pit != (*lit)->logs.end())
		{
			free(*pit);
			pit++;
		}
		delete (*lit);
		lit++;
	}
	format_file.log_types.clear();
}

int main(int argc, char* argv[])
{
	if(argc < 2)
		return -1;
	xmlreader xmlread;
	xmlread.processFile(argv[1]);
	xmlread.printOut();
	return 0;
}

#endif

