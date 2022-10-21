#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "AST.h"

int compare(AST_T* a, AST_T* b)
{
    if (a->type != b->type)
        return 0;
    switch (a->type)
    {
        case AST_STRING: return !strcmp(a->string_value, b->string_value);
        case AST_NUMBER: return a->ast_number == b->ast_number;
        case AST_BOOLEAN: return a->boolean_value == b->boolean_value;
    }
    return 0;
}