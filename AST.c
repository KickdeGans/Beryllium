#include "AST.h"


AST_T* init_ast(int type)
{
    AST_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->type = type;
  
    ast->scope = (void*) 0;
    ast->private_scope = (void*) 0;
  
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

    /*  AST_VARIABLE */
    ast->variable_name = (void*) 0;
    ast->statement_value = (void*) 0;

    /*  AST_FUNCTION_CALL */
    ast->function_call_name = (void*) 0;
    ast->function_call_arguments = (void*) 0;
    ast->function_call_arguments_size = 0;
  
    /*  AST_STRING */
    ast->string_value = (void*) 0;
  
    /* AST_BOOLEAN */
    ast->boolean_variable_a = (void*) 0;
    ast->boolean_variable_b = (void*) 0;
    ast->boolean_type = 0;
    ast->boolean_value = 0;

    /* AST_NUMBER */
    ast->ast_number = 0;

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

    return ast;
}