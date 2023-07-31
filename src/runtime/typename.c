#include "typename.h"
#include "../core/AST.h"

char* type_name(int type)
{
    switch (type)
    {
        case AST_STRING: return "string";
        case AST_INT: return "int";
        case AST_DOUBLE: return "double";
        case AST_BOOLEAN: return "bool";
        case AST_ARRAY: return "array";
        case AST_STREAM: return "stream";
    }

    return "unknown";
}