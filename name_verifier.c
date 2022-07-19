#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "AST.h"

char allowed_chars[] =
{
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_', '.'
};

int name_verifier_is_valid_var_name(char* name)
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
            }
        }
        if (valid == 0)
        {
            return 0;
        }
    }
    return 1;
}
int digits_only(const char *s)
{
    while (*s) 
    {
        if (isdigit(*s++) == 0)
        { 
            return 0;
        }
    }
    return 1;
}