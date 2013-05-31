#include "filereader.hpp"

int main(int argc, char* argv[])
{
	if(argc < 3)
		return -1;
	
	filereader freader(argv[1], argv[2]);
	freader.parse_fmt_file();
	freader.parse_data_file();
	return 0;
}

