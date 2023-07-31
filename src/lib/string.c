#include "string.h"
#include <stdio.h>
#include <string.h>

char* replace_char(char* str, char find, char replace)
{
    char *current_pos = strchr(str,find);
    while (current_pos) 
    {
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}
int contains_char(char* str, char find)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == find)
            return 1;
    }
    return 0;
}
int fast_compare(const char *str, const char *cmp)
{
    while (*str) 
        if (*str++ != *cmp++)
            return 1;
    return 0;
}