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
        case AST_VARIABLE: return "variable";
        case AST_VARIABLE_DEFINITION: return "variable_definition";
        case AST_VARIABLE_SETTER: return "variable_setter";
        case AST_COMPOUND: return "compound";
        case AST_ATTRIBUTE: return "attribute";
        case AST_DICT_ITEM: return "dict_item";
        case AST_FUNCTION_CALL: return "function_call";
        case AST_FUNCTION_DEFINITION: return "function_definition";
        case AST_FUNCTION_RETURN: return "return_value";
        case AST_GET_ARRAY_ITEM_BY_INDEX: return "get_array_item_by_index";
        case AST_MATH_EXPR: return "math_expression";
        case AST_NOOP: return "null";
        case AST_STATEMENT_CALL: return "statement_call";
        case AST_STATEMENT_DEFINITION: return "statement_definition";
    }

    return "unknown";
}