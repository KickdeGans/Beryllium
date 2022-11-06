#ifndef AST_H
#define AST_H
#include <stdlib.h>

typedef struct AST_STRUCT
{
    enum {
        AST_VARIABLE_DEFINITION,
        AST_FUNCTION_DEFINITION,
        AST_STATEMENT_DEFINITION,
        AST_STRUCT_DEFINITION,
        AST_CLASS_DEFINITION,
        AST_VARIABLE_SETTTER,
        AST_VARIABLE,
        AST_FUNCTION_CALL,
        AST_STATEMENT_CALL,
        AST_STRING,
        AST_BOOLEAN,
        AST_INT,
        AST_DOUBLE,
        AST_FORLOOP,
        AST_FUNCTION_RETURN,
        AST_COMPOUND,
        AST_NOOP,
        AST_ARRAY,
        AST_GET_ARRAY_ITEM_BY_INDEX,
        AST_DICT_ITEM,
        AST_MATH_EXPR
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
    struct SCOPE_STRUCT* private_scope;
    int is_return_value;
    int break_;
    int continue_;

    /* AST_VARIABLE_DEFINITION */
    char* variable_definition_variable_name;
    struct AST_STRUCT* variable_definition_value;
    int variable_definition_is_public;
    int variable_definition_is_const;
    int variable_definition_value_type;

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

    /* AST_STRUCT_DEFINITION */
    char* struct_definition_name;
    struct AST_STRUCT** struct_definition_content;
    size_t struct_definition_content_size;

    /* AST_VARIABLE */
    char* variable_name;
    char* statement_value;

    /* AST_FUNCTION_CALL */
    char* function_call_name;
    struct AST_STRUCT** function_call_arguments;
    size_t function_call_arguments_size;

    /* AST_STATEMENT_CALL */
    char* statement_call_type;
    struct AST_STRUCT* statement_call_argument;

    /* AST_STRING */
    char* string_value;

    /* AST_BOOLEAN */
    struct AST_STRUCT* boolean_variable_a;
    struct AST_STRUCT* boolean_variable_b;
    struct AST_STRUCT** boolean_expr;
    int boolean_operator;
    int boolean_value;

    /* AST_INT */
    int ast_int;

    /* AST_DOUBLE */
    double ast_double;

    /* AST_FORLOOP */
    char* forloop_variable_name;
    struct AST_STRUCT* forloop_value;

    /* AST_VARIABLE_SETTER */
    char* variable_setter_variable_name;
    struct AST_STRUCT* variable_setter_value;

    /* AST_COMPOUND */
    struct AST_STRUCT** compound_value;
    size_t compound_size;

    /* AST_ARRAY*/
    struct AST_STRUCT** array_value;
    size_t array_size;

    /* AST_GET_ARRAY_ITEM_BY_INDEX */
    struct AST_STRUCT* array_item;
    struct AST_STRUCT* array_index;

    /* AST_DICT_ITEM */
    char* dict_key;
    struct AST_STRUCT* dict_value;

    /* AST_MATH_EXPR */
    struct AST_STRUCT** math_expression;
    size_t math_expression_size;
    struct AST_STRUCT* math_expression_value;
    char math_expression_type;

} AST_T;

AST_T* init_ast(int type);

#endif
