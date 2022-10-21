#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "AST.h"
#include "identifier.h"

char allowed_chars[] =
{
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'n', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_', '.'
};

int is_valid_name(char* name)
{
    if (digits_only(name))
    {
        return 0;
    }
    int valid = 1;
    for (int i = 0; i < sizeof(name); i++)
    {
        for (int j = 0; j < sizeof(allowed_chars); j++)
        {
            if (allowed_chars[j] == name[i])
            {
                valid = 1;
                break;
            }
        }
    }
    return valid;
}
int is_allowed_char(char ch)
{
    int valid = 0;
    for (int i = 0; i < sizeof(allowed_chars); i++)
    {
        if (ch == allowed_chars[i])
        {
            valid = 1;
            break;
        }
    }
    return valid;
}

int digits_only(const char *s)
{
    while (*s) 
    {
        char c = *s++;
        if (c == '\0')
        {
            return 1;
        }
        if (isdigit(c) == 0 || c != '.')
        { 
            return 0;
        }
    }
    return 1;
}