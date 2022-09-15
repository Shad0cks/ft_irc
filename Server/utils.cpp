#include "includes.hpp"
#include <string>

int strfcr(std::string  src, char c)
{
    for(int i = 0; i < src.size(); i++)
    {
        if (src[i] == c)
            return(i);
    }
    return (0);
}