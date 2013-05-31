#include "xmlreader.hpp"

int main(int argc, char* argv[])
{
	if(argc < 2)
		return -1;
	xmlreader xmlread;
	xmlread.processFile(argv[1]);
	xmlread.printOut();
	xmlread.cleanup();
	return 0;
}

