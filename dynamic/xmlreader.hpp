#ifndef __XML_READER_H__
#define __XML_READER_H__

#include <vector>
#include <libxml2/libxml/xmlreader.h>
using namespace std;

typedef unsigned int u32;

struct PARA_entity;
// length type
typedef enum length_t {
	BIT, BYTE, OTHER_BIT, OTHER_BYTE
}length_t;
typedef struct length
{
	PARA_entity* e;
	u32 l;
}length;
// range type
typedef enum range_t {
	T_VALUE, T_RANGE, T_ANY
}range_t;
typedef struct range
{
	range_t type;
	long low, high;
}range;
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
		length_t len_t;
		length len;
		PARA_entity* depend; // nil if not have depend 
		range rng; // only in para choice value = 
	}attr;
}PARA_entity;

typedef struct log_t
{
	range rng;  // <LOG value=$rng>
	vector<PARA_entity*> logs;
}log_t;
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
	void processFile(const char* file);
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

