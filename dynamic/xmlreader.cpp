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

#include "xmlreader_static.hpp"

/**
 * process one XML node
 */
void xmlreader::processNode(xmlTextReaderPtr reader)
{
	const xmlChar *name, *s;
	static PARA_entity entity;
	// type
	int type = xmlTextReaderNodeType(reader);
	if(type != XML_READER_TYPE_ELEMENT)
		return;
	memset(&entity, 0, sizeof(PARA_entity));
	// depth
	entity.depth = xmlTextReaderDepth(reader);
	// <name attr=""></name>
	name = xmlTextReaderConstName(reader);
	if(name == NULL)
	 	name = BAD_CAST "nil";
	if(xmlStrncasecmp(name, AREADESC, MLEN) == 0)// 0 <AREADESC>
	{
		printf("**Processing <AREADESC> ...\n");
		assert(entity.depth == 0);
		return;
	}
	else if(xmlStrncasecmp(name, FILEHEAD, MLEN) == 0)// 1 <FILEHEAD>
	{
		printf("***Processing <FILEHEAD> ...\n");
		assert(entity.depth == 1);
		processing = &format_file.file_head;
		return;
	}
	else if(xmlStrncasecmp(name, LOGHEAD, MLEN) == 0)// 1 <LOGHEAD>
	{
		printf("***Processing <LOGHEAD> ...\n");
		assert(entity.depth == 1);
		processing = &format_file.log_head;
		return;
	}
	else if(xmlStrncasecmp(name, LOGTYPE, MLEN) == 0)// 1 <LOGTYPE>
	{
		printf("***Processing <LOGTYPE> ...\n");
		assert(entity.depth == 1);
		return;
	}
	else if(xmlStrncasecmp(name, LOG, MLEN) == 0)// 2 <LOG type="" ...
	{
		printf("****Processing <LOG type=");
		assert(entity.depth == 2);
		assert(xmlTextReaderAttributeCount(reader) > 0);
		log_t *one_log_type = (log_t *)malloc(sizeof(log_t));
		// setup range
		s = xmlTextReaderGetAttributeNo(reader, 0);
		resolveRng(one_log_type->rng, s);
		format_file.log_types.push_back(one_log_type);
		printf("'%s'> ...\n", s);
		processing = &one_log_type->logs;
		return;
	}
	else if(xmlStrncasecmp(name, PARA, MLEN) == 0)// <PARA ...
	{
		entity.type = T_PARA;
		assert(xmlTextReaderAttributeCount(reader) > 0);
		int count = xmlTextReaderAttributeCount(reader);
		for(int i = 0; i < count; ++i)
		{
			s = xmlTextReaderGetAttributeNo(reader, i);
			if(i == 0)// name=""
				entity.name = xmlStrdup(s);
			else if(i == 1)// type=""
				entity.attr.type = atoi((const char*)s);
			else if(i == 2)//length ?? depend
				entity.attr.len.l = atoi((const char*)s);
			else if(i == 3)//??
				;
			else
				;
		}
		
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
			for(rit=processing->rbegin(); rit!=processing->rend(); ++rit)
			{
				if((*rit)->depth == entity.depth-1)
				{
					entity.attr.depend = *rit;
					break;
				}
			}
			processing->push_back(dup_PARA_entity(&entity));
		}
		else
		{
			//when a PARACHOICE without 'value=', the PARA before it is useless
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

int xmlreader::processFile(const char* file)
{
	int ret;
	xmlTextReaderPtr reader;
	reader = xmlReaderForFile(file, NULL, XML_PARSE_DTDATTR | XML_PARSE_RECOVER);
	if(reader != NULL)
	{
		processing = NULL;
		cleanup();
		ret = xmlTextReaderRead(reader);
		while (ret == 1) // process each node in XML file
		{
			processNode(reader);
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
		if(ret != 0)
		{
			fprintf(stderr, "Error in parse %s\n", file);
			return ret;
		}
		return 0;
	}
	else
	{
		fprintf(stderr, "Open %s error\n", file);
		return 1;
	}
}

void xmlreader::printOut()
{
	vector<PARA_entity*>::iterator it;
	vector<log_t*>::iterator lit;
	printf("======== Output read in to check ========\n");
	printf("-----<File head info(%d)>-----\n", format_file.file_head.size());
	it = format_file.file_head.begin();
	for(;it != format_file.file_head.end();it++)
		show_PARA_entity(*it);
	printf("-----<Log head info(%d)>-----\n", format_file.log_head.size());
	it = format_file.log_head.begin();
	for(;it != format_file.log_head.end();it++)
		show_PARA_entity(*it);
	printf("-----<Log type info(%d)>-----\n", format_file.log_types.size());
	lit = format_file.log_types.begin();
	for(;lit != format_file.log_types.end();lit++)
		show_one_log_type(*lit);
	printf("========= Finish output read in =========\n");
}

void xmlreader::cleanup()
{
	vector<PARA_entity*>::iterator it;
	for(it = format_file.file_head.begin(); it != format_file.file_head.end(); ++it)
		free_PARA_entity(*it);
	for(it = format_file.log_head.begin(); it != format_file.log_head.end(); ++it)
		free_PARA_entity(*it);
	vector<log_t*>::iterator lit = format_file.log_types.begin();
	while(lit != format_file.log_types.end())
	{
		for(it = (*lit)->logs.begin(); it != (*lit)->logs.end(); ++it)
			free_PARA_entity(*it);
		free(*lit);
		++lit;
	}
	format_file.file_head.clear();
	format_file.log_head.clear();
	format_file.log_types.clear();
}

#endif

