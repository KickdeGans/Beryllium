#include "AST.h"


AST_T* init_ast(int type)
{
    AST_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->type = type;
  
    ast->scope = (void*) 0;
    ast->private_scope = (void*) 0;
    ast->is_return_value = 0;
    ast->break_ = 0;
    ast->continue_ = 0;
  
    /*  AST_VARIABLE_DEFINITION */
    ast->variable_definition_variable_name = (void*) 0;
    ast->variable_definition_value = (void*) 0;
    ast->variable_definition_is_public = -1;
    ast->variable_definition_is_const = 0;
  
    /*  AST_FUNCTION_DEFINITION */
    ast->function_definition_body = (void*) 0;
    ast->function_definition_name = (void*) 0;
    ast->function_definition_args = (void*) 0;
    ast->function_definition_args_size = 0;
    
    /* AST_STATEMENT_DEFINITION */
    ast->statement_definition_body = (void*) 0;
    ast->statement_definition_type = (void*) 0;
    ast->statement_definition_args = (void*) 0;
    ast->statement_definition_args_size = 0;

    /* AST_STRUCT_DEFINITION */
    ast->struct_definition_name = (void*) 0;
    ast->struct_definition_content = (void*) 0;
    ast->struct_definition_content_size = 0;

    /*  AST_VARIABLE */
    ast->variable_name = (void*) 0;
    ast->statement_value = (void*) 0;

    /*  AST_FUNCTION_CALL */
    ast->function_call_name = (void*) 0;
    ast->function_call_arguments = (void*) 0;
    ast->function_call_arguments_size = 0;

    /* AST_STATEMENT_CALL */
    ast->statement_call_type = (void*) 0;
    ast->statement_call_argument = (void*) 0;
  
    /*  AST_STRING */
    ast->string_value = (void*) 0;
  
    /* AST_BOOLEAN */
    ast->boolean_variable_a = (void*) 0;
    ast->boolean_variable_b = (void*) 0;
    ast->boolean_type = 0;
    ast->boolean_value = 0;

    /* AST_INT */
    ast->ast_int = 0;

    /* AST_DOUBLE */
    ast->ast_double = 0;

    /* AST_LONG */
    ast->ast_long = 0;

    /* AST_CHAR */
    ast->ast_char = '\0';

    /* AST_VARIABLE_SETTER */
    ast->variable_setter_variable_name = (void*) 0;
    ast->variable_setter_value = (void*) 0;

    /*  AST_COMPOUND */
    ast->compound_value = (void*) 0;
    ast->compound_size = 0;

    /* AST_FORLOOP */
    ast->forloop_variable_name = (void*) 0;

    /* AST_ARRAY */
    ast->array_value = (void*) 0;
    ast->array_size = 0;

    /* AST_DICT_ITEM */
    ast->dict_key = (void*) 0;
    ast->dict_value = (void*) 0;

    /* AST_MATH_EXPR */
    ast->math_expression = (void*) 0;
    ast->math_expression_size = 0;
    ast->math_expression_type = 0;
    ast->math_expression_value = (void*) 0;

    return ast;
}


void ast_free(AST_T* ast)
{
    if (ast != (void*)0)
        free(ast);
    return;
}