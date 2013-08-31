#include <iostream>
#include "bwt.h"

int main(int argc, char *argv[])
{
     const std::string target = "banana";
     bwt b;
     const std::string bw_transformed = b.transform(target);
     std::cout << "==================================" << std::endl;
     std::cout << bw_transformed << std::endl;
     std::cout << "==================================" << std::endl;
     std::cout << b.inverse(bw_transformed) << std::endl;
     return 0;
}

