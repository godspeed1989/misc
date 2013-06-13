#include "filereader.hpp"

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		printf("Usage: %s xml_file data_file\n", argv[0]);
		return -1;
	}
	
	filereader freader(argv[1], argv[2]);
	if(freader.parse_fmt_file())
	{
		printf("error in parse XML file %s\n", argv[1]);
		return -1;
	}
	if(freader.parse_data_file() < 0)
	{
		printf("error in parse data file %s\n", argv[2]);
		return -1;
	}
	freader.summary();
	freader.dump_all("dump.hex");
	return 0;
}

