#include "xmlreader.hpp"
#include <cstdio>
#include <cassert>
#include <cstring>

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
	PARA_entity entity;
	const xmlChar *node_name, *s;
	memset(&entity, 0, sizeof(PARA_entity));
	// XML node type
	int type = xmlTextReaderNodeType(reader);
	if(type != XML_READER_TYPE_ELEMENT) // use <node> not <node/>
	{
		return;
	}
	// XML node depth
	entity.depth = xmlTextReaderDepth(reader);
	// <node_name attr=""></node_name>
	node_name = xmlTextReaderConstName(reader);
	if(node_name == NULL)
	{
		return;
	}
	// process the XML node depend on its name
	if(xmlStrncasecmp(node_name, AREADESC, MLEN) == 0)// 0 <AREADESC>
	{
		printf("**Processing <%s> ...\n", AREADESC);
		assert(entity.depth == 0);
		return;
	}
	else if(xmlStrncasecmp(node_name, FILEHEAD, MLEN) == 0)// 1 <FILEHEAD>
	{
		printf("***Processing <%s> ...\n", FILEHEAD);
		assert(entity.depth == 1);
		processing = &format_file.file_head;
		return;
	}
	else if(xmlStrncasecmp(node_name, LOGHEAD, MLEN) == 0)// 1 <LOGHEAD>
	{
		printf("***Processing <%s> ...\n", LOGHEAD);
		assert(entity.depth == 1);
		processing = &format_file.log_head;
		return;
	}
	else if(xmlStrncasecmp(node_name, LOGTYPE, MLEN) == 0)// 1 <LOGTYPE>
	{
		printf("***Processing <%s> ...\n", LOGTYPE);
		assert(entity.depth == 1);
		return;
	}
	else if(xmlStrncasecmp(node_name, LOG, MLEN) == 0)// 2 <LOG type="" ...
	{
		printf("****Processing LOG type=");
		assert(entity.depth == 2);
		assert(xmlTextReaderAttributeCount(reader) > 0);
		log_format *one_log_fmt = new log_format;
		// get "type=" attr to setup range
		s = xmlTextReaderGetAttributeNo(reader, 0);
		resolve_range(one_log_fmt->rng, s);
		show_range(one_log_fmt->rng, stdout);
		// output describe infomation if exist
		s = xmlTextReaderGetAttributeNo(reader, 1);
		if(s)
			printf(" '%s'", s);
		printf("\n");
		format_file.log_fmt.push_back(one_log_fmt);
		processing = &one_log_fmt->entities;
		return;
	}
	else if(xmlStrncasecmp(node_name, PARA, MLEN) == 0)// <PARA name="" ...
	{
		entity.type = T_PARA;
		assert(xmlTextReaderAttributeCount(reader) > 0);
		// parse attributes one by one
		int count = xmlTextReaderAttributeCount(reader);
		for(int i = 0; i < count; ++i) //TODO: get detailed format information
		{
			s = xmlTextReaderGetAttributeNo(reader, i);
			if(i == 0)
			{
				entity.name = xmlStrdup(s); // name=""
			}
			else if(i == 1)
			{
				entity.attr.type = atoi((const char*)s); // type=""
			}
			else if(i == 2)
			{	
				// case length=""
				switch(entity.attr.type)
				{
					T_BYTE_CASE:
						entity.attr.len.lb = atoi((const char*)s) << 3;
						break;
					T_BIT_CASE:
						entity.attr.len.lb = atoi((const char*)s);
						break;
					T_BYTE_REF_CASE: T_BIT_REF_CASE:
						entity.attr.len.lb = -1;
						entity.attr.len.le = get_ref_by_name(processing, s);
						break;
					default:
						printf("unknow attr type %d of %s\n", entity.attr.type, entity.name);
						throw;
				}
			}
			else if(i == 3)
			{
				;//TODO
			}
			else
			{
				;//TODO
			}
		}
		processing->push_back(dup_PARA_entity(&entity));
	}
	else if(xmlStrncasecmp(node_name, PARACHOICE, MLEN) == 0)// <PARACHOCE value="">
	{
		entity.type = T_PARACHOICE;
		if(xmlTextReaderAttributeCount(reader) > 0)
		{
			// value= a range
			s = xmlTextReaderGetAttributeNo(reader, 0);
			resolve_range(entity.attr.rng, s);
			// backward find first PARA with depth less 1
			vector<PARA_entity*>::reverse_iterator rit;
			for(rit=processing->rbegin(); rit!=processing->rend(); ++rit)
			{
				if((*rit)->depth == entity.depth-1)
				{
					entity.depend = *rit;
					break;
				}
			}
			if(rit == processing->rend())
			{
				printf("can't find PARACHOICE value=%s dependency\n", s);
				throw;
			}
			processing->push_back(dup_PARA_entity(&entity));
		}
		else // no attribute
		{
			// TODO make sure : when a PARACHOICE *without* "value="
			// the one PARA before it is useless
			free_PARA_entity(processing->back());
			processing->pop_back();
			return;
		}
	}
	else// <UNKNOWN ...
	{
		printf("Unknow XML node name [%s]\n", node_name);
		exit(-1);
	}
	
#if 0
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

/*
 * parse and read in XML format file
 */
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

// print out the XML file content
void xmlreader::printOut(FILE *fout)
{
	vector<PARA_entity*>::iterator it;
	vector<log_format*>::iterator lit;
	fprintf(fout, "======== Output read in to check ========\n");
	fprintf(fout, "-----<File head info(%zu)>-----\n", format_file.file_head.size());
	for(it = format_file.file_head.begin(); it != format_file.file_head.end(); ++it)
		show_PARA_entity(*it, fout);
	fprintf(fout, "-----<Log head info(%zu)>-----\n", format_file.log_head.size());
	for(it = format_file.log_head.begin(); it != format_file.log_head.end(); ++it)
		show_PARA_entity(*it, fout);
	fprintf(fout, "-----<Log type info(%zu)>-----\n", format_file.log_fmt.size());
	for(lit = format_file.log_fmt.begin(); lit != format_file.log_fmt.end(); ++lit)
		show_one_log_fmt(*lit, fout);
	fprintf(fout, "========= Finish output read in =========\n");
}

// clean up function
void xmlreader::cleanup()
{
	vector<PARA_entity*>::iterator it;
	vector<log_format*>::iterator lit;
	for(it = format_file.file_head.begin(); it != format_file.file_head.end(); ++it)
		free_PARA_entity(*it);
	for(it = format_file.log_head.begin(); it != format_file.log_head.end(); ++it)
		free_PARA_entity(*it);
	for(lit = format_file.log_fmt.begin(); lit != format_file.log_fmt.end(); ++lit)
	{
		for(it = (*lit)->entities.begin(); it != (*lit)->entities.end(); ++it)
			free_PARA_entity(*it);
		delete (*lit);
	}
	format_file.file_head.clear();
	format_file.log_head.clear();
	format_file.log_fmt.clear();
}

#endif

