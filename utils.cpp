#include "utils.hpp"

std::string     trim(std::string str)
{
    int         end;

    end = 0;
    while ((str[end] != ' ') && (str[end] != '\t') && (str[end] != '\n') && (str[end] != '\v') && (str[end] != '\f') && (str[end] != '\r'))
    {
        end++;
    }
    str.erase(end, str.size());
    return (str);
}