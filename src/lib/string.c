#include "string.h"
#include <stdio.h>
#include <string.h>

char* replace_char(char* str, char find, char replace)
{
    char* current_pos = strchr(str,find);
    while (current_pos) 
    {
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}
int contains_char(const char* str, const char find)
{
    while (*str) 
        if (*str++ == find)
            return 1;
    return 0;
}

//faster implementation of strcmp to only check if two strings match
int fast_compare(const char *str, const char *cmp)
{
    while (*str) 
        if (*str++ != *cmp++)
            return 1;
    return *cmp != 0;
}