#ifndef __XML_READER_H__
#define __XML_READER_H__

#include <vector>
#include <libxml2/libxml/xmlreader.h>
using namespace std;

#define MLEN         128

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
// PARA's length, cound be a constant value or depend on another
typedef struct length
{
	int lb;
	const PARA_entity* le;
}length;

// PARA type
typedef enum { T_PARA, T_PARACHOICE } PARA_entity_t;
// PARA entity
typedef struct PARA_entity
{
	int depth;
	const xmlChar * name;
	PARA_entity_t type;
	const PARA_entity * depend;  // used by "PARACHOICE" or "depend="
	struct attr
	{
		int type;        // type= in attr
		length len;
		range rng;       // only in para choice "value=1~3"
	}attr;
}PARA_entity;

// structure of each log
typedef struct log_format
{
	range rng;               // <LOG value=$rng>
	vector<PARA_entity*> entities;
}log_format;
// structure of the data file
typedef struct file_format
{
	vector<PARA_entity*> file_head;
	vector<PARA_entity*> log_head;
	vector<log_format*> log_fmt;
}file_format;

typedef class xmlreader
{
public:
	file_format format_file;
	int processFile(const char* file);
	void printOut(FILE *fout);
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

// the "length=" attribute's type
#define T_BIT_CASE          case 3
#define T_BYTE_CASE         case 0 : case 4 : case 16
#define T_BIT_REF_CASE      case 7
#define T_BYTE_REF_CASE     case 11

#endif

