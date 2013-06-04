#ifndef __XML_READER_H__
#define __XML_READER_H__

#include <vector>
#include <libxml2/libxml/xmlreader.h>
using namespace std;

typedef unsigned int u32;

// range type
typedef enum range_t
{
	T_VALUE, T_RANGE, T_ANY
}range_t;
typedef struct range
{
	range_t type;
	long low, high;
}range;

struct PARA_entity;
// length, cound be a instant value or value of others
typedef struct length
{
	PARA_entity* e;
	u32 l;
}length;

// PARA type
typedef enum { T_PARA, T_PARACHOICE } PARA_entity_t;
// PARA entity
typedef struct PARA_entity
{
	int depth;
	xmlChar* name;
	PARA_entity_t type;
	struct attr
	{
		int type; // type= in attr
		length len;
		PARA_entity* depend; // nil if don't have dependence
		range rng; // only in para choice "value="
	}attr;
}PARA_entity;

// structure of each log
typedef struct log_t
{
	range rng;  // <LOG value=$rng>
	vector<PARA_entity*> logs;
}log_t;
// structure of the data file
typedef struct file_format
{
	vector<PARA_entity*> file_head;
	vector<PARA_entity*> log_head;
	vector<log_t*> log_types;
}file_format;

typedef class xmlreader
{
public:	
	file_format format_file;
	int processFile(const char* file);
	void printOut();
	void cleanup();
	xmlreader()
	{
		LIBXML_TEST_VERSION
	}
	~xmlreader()
	{
		cleanup();
		// cleanup function for the XML library
		xmlCleanupParser();
	}
private:
	vector<PARA_entity*>* processing;
	void processNode(xmlTextReaderPtr reader);
}xmlreader;

#endif

