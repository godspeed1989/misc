#include <cstdio>
#include <libxml2/libxml/xmlreader.h>

#ifdef LIBXML_READER_ENABLED

void processNode(xmlTextReaderPtr reader)
{
	int i, count, type;
	const xmlChar *name, *value, *attr;
	
	// depth
	printf("depth=%d ", xmlTextReaderDepth(reader));
	// type
	type = xmlTextReaderNodeType(reader);
	printf("type=%d ", type);
	// <name attr=""></name>
	name = xmlTextReaderConstName(reader);
	if(name == NULL)
	 	name = BAD_CAST "nil";
	printf("name='%s' ", name);
	// <a></a>  or <node/>
	if(xmlTextReaderIsEmptyElement(reader))
		printf("empty_ele ");
	// <a>value</a>
	if(type != XML_READER_TYPE_SIGNIFICANT_WHITESPACE && 
		xmlTextReaderHasValue(reader))
	{
		value = xmlTextReaderConstValue(reader);
		count = xmlStrlen(value);
		if(count > 40)
			printf("value='%.40s'... ", value);
		else if(count > 0)
			printf("value='%s' ", value);
	}
	// <name attr1="" attr2=""/>
	if(type != XML_READER_TYPE_END_ELEMENT &&
		xmlTextReaderAttributeCount(reader) > 0)
	{
		printf("\n[");
		count = xmlTextReaderAttributeCount(reader);
		for(i=0; i<count; i++)
		{
			xmlTextReaderGetAttribute(reader, name);
			attr = xmlTextReaderGetAttributeNo(reader, i);
			printf(" %s='%s' ", name, attr);
		}
		printf("]");
	}
	
	printf("\n");
}

void streamFile(const char* file)
{
	int ret;
	xmlTextReaderPtr reader;
	reader = xmlReaderForFile(file, NULL, 0);
	if(reader != NULL)
	{
		ret = xmlTextReaderRead(reader);
		while (ret == 1)
		{
			processNode(reader);
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
		if(ret != 0) {
			fprintf(stderr, "Error in parse %s\n", file);
		}
	}
	else
	{
		fprintf(stderr, "Open %s error\n", file);
	}
}

int main(int argc, char* argv[])
{
	if(argc < 2)
		return -1;
	LIBXML_TEST_VERSION
	
	streamFile(argv[1]);
	
	return 0;
}

#else
int main()
{
	fprintf(stderr, "xml reader not compiled in.\n");
	return -1;
}
#endif

