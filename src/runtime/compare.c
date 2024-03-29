#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../core/AST.h"
#include "../lib/string.h"

/* Compare two values of the same type */
int compare(AST_T* a, AST_T* b)
{
    if (a->type != b->type)
        return 0;
    switch (a->type)
    {
        case AST_STRING: return fast_compare(a->string_value, b->string_value) == 0;
        case AST_INT: return a->ast_int == b->ast_int;
        case AST_DOUBLE: return a->ast_double == b->ast_double;
        case AST_BOOLEAN: return a->boolean_value == b->boolean_value;
        default: break;
    }
    return 0;
}