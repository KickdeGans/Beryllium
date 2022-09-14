#ifndef AST_H
#define AST_H
#include <stdlib.h>

typedef struct AST_STRUCT
{
    enum {
        AST_VARIABLE_DEFINITION,
        AST_FUNCTION_DEFINITION,
        AST_STATEMENT_DEFINITION,
        AST_VARIABLE,
        AST_FUNCTION_CALL,
        AST_STRING,
        AST_BOOLEAN,
        AST_FORLOOP,
        AST_FUNCTION_RETURN,
        AST_COMPOUND,
        AST_NOOP
    } type;

    enum {
        BOOLEAN_EQUALTO,
        BOOLEAN_NOTEQUALTO,
        BOOLEAN_GREATERTHAN,
        BOOLEAN_LESSTHAN,
        BOOLEAN_EGREATERTHAN,
        BOOLEAN_ELESSTHAN
    } boolean_type;

    struct SCOPE_STRUCT* scope;

    /* AST_VARIABLE_DEFINITION */
    char* variable_definition_variable_name;
    struct AST_STRUCT* variable_definition_value;

    /* AST_FUNCTION_DEFINITION */
    struct AST_STRUCT* function_definition_body;
    char* function_definition_name;
    struct AST_STRUCT** function_definition_args;
    size_t function_definition_args_size;

    /* AST_STATEMENT_DEFINITION */
    struct AST_STRUCT* statement_definition_body;
    char* statement_definition_type;
    struct AST_STRUCT** statement_definition_args;
    size_t statement_definition_args_size;

    /* AST_VARIABLE */
    char* variable_name;
    char* statement_value;

    /* AST_FUNCTION_CALL */
    char* function_call_name;
    struct AST_STRUCT** function_call_arguments;
    size_t function_call_arguments_size;

    /* AST_STRING */
    char* string_value;

    /* AST_BOOLEAN */
    struct AST_STRUCT* boolean_variable_a;
    struct AST_STRUCT* boolean_variable_b;
    int boolean_operator;
    int boolean_value;

    /* AST_FORLOOP */
    char* forloop_variable_name;
    struct AST_STRUCT** forloop_array;

    /* AST_COMPOUND */
    struct AST_STRUCT** compound_value;
    size_t compound_size;
} AST_T;

AST_T* init_ast(int type);
#endif
