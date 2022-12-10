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

/* Check if a identifier name is valid */
/* Returns 0 if not */
/* Returns 1 if it is */
int is_valid_name(const char* name)
{
    if (digits_only(name))
    {
        return 0;
    }
    int valid = 1;
    for (size_t i = 0; i < sizeof(name); i++)
    {
        for (size_t j = 0; j < sizeof(allowed_chars); j++)
        {
            if (name[i] == allowed_chars[j])
            {
                valid = 1;
            }
        }
    }
    return valid;
}

/* Determines if a character is allowed for a name */
int is_allowed_char(const char ch)
{
    int valid = 0;
    for (size_t i = 0; i < sizeof(allowed_chars); i++)
    {
        if (ch == allowed_chars[i])
        {
            valid = 1;
            break;
        }
    }
    return valid;
}

/* Checks if a string contains only digits */
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