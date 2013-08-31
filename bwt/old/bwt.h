#ifndef BURROWS_WHEELER_H
#define BURROWS_WHEELER_H

#include <string>
#include <map>
#include <algorithm>

class bwt
{
    private:
        static const char final_char;
        std::string lf_mapping(const std::string &text);
        std::string basic_inv(const std::string &text);
    public:
        std::string transform(const std::string &text);
        std::string inverse(const std::string &text);
};

const char bwt::final_char = '$';

std::string bwt::transform(const std::string &text) {
    const unsigned int length = text.length() + 1; //1 means final_char.length()
    std::string block[length];

    for (int i=0;i<length;++i)
    {
        block[i] = text.substr(i,length) + final_char + text.substr(0, i);
    }
    sort(block, block + length);
    std::string transformed = "";
    for (int i=0;i<length;++i)
    {
        std::cout << block[i] << std::endl;
        transformed += block[i][length - 1];
    }
    return transformed;
}

std::string bwt::inverse(const std::string &text) {
    return lf_mapping(text);
}

std::string bwt::lf_mapping(const std::string &L) {
    std::string F = std::string(L.begin(), L.end());
    sort(F.begin(), F.end());

    std::map<char, int> C;
    int occ[L.length()];
    int LF[F.length()];
    {
        unsigned int occ = 0;
        std::string::const_iterator it = F.begin();
        const std::string::const_iterator end = F.end();
        for(int i=0;it != end;++it, ++i) {
            if (C.count(*it) == 0) {
                C[*it] = i;
            }
        }
    }
    {
        std::map< char, int> occ;
        std::string::const_iterator it_L = L.begin();
        const std::string::const_iterator end = L.end();
        for(int i=0;it_L != end;++it_L, ++i) {
            if (occ.count(*it_L) == 0) {
                occ[*it_L] = 0;
            }
            ++occ[*it_L];
            LF[i] = C[*it_L] + occ[*it_L] - 1; // -1 means index offset (1,2,3,...) to (0,1,2,...)
            std::cout << *it_L << "," << C[*it_L] << "," << occ[*it_L] << "," << LF[i] << std::endl;
        }

    }

    unsigned int final_char_pos = 0;
    {
        std::string::const_iterator it = L.begin();
        const std::string::const_iterator end = L.end();
        for(;it != end;++it, ++final_char_pos) {
            if (*it == final_char) {
                break;
            }
        }
    }
    std::cout << final_char_pos << std::endl;
    
    std::string result = "";
    {
        const int end = L.length();
        for(int i=0, pos=final_char_pos;i<end;++i)
        {
            result = L[pos] + result;
            pos = LF[pos] ;
        }
    }
    return result;
}

#endif

