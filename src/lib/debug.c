#include "debug.h"
#include "../runtime/typename.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

void AST_analysis(AST_T* ast)
{
    printf("\n\n####### BEGIN AST ANALYSIS #######\n\n");

    printf("type: %i\n", ast->type);
    printf("possible type: %s\n\n", type_name(ast->type));

    printf("scope addr: ");
    printf("0x%" PRIXPTR "\n", (uintptr_t)ast->scope);
    printf(!ast->scope ? "It appears the scope is NULL!\n\n": "\n\n");

    printf("private scope addr: ");
    printf("0x%" PRIXPTR "\n", (uintptr_t)ast->private_scope);
    printf(!ast->private_scope ? "It appears the private_scope is NULL!\n\n" : "\n\n");

    printf("is_private: %i\n\n", ast->is_private);

    printf("string val: %s\n", ast->string_value);
    printf("int val %i\n", ast->ast_int);
    printf("is_stream_null: %i\n\n", ast->stream ? 1 : 0);

    printf("break_: %i\n", ast->break_);
    printf("continue_: %i\n\n", ast->continue_);

    printf("function call name: %s\n\n", ast->function_call_name);

    printf("variable_definition_variable_name: %s\n", ast->variable_definition_variable_name);
    printf("variable_definition_is_const: %i\n", ast->variable_definition_is_const);
    printf("variable_definition_is_public: %i\n", ast->variable_definition_is_public);
    printf("variable_name: %s\n", ast->variable_name);
    printf("variable_setter_variable_name: %s\n\n", ast->variable_setter_variable_name);

    printf("statement_definition_type: %s\n\n", ast->statement_definition_type);

    printf("boolean_operator; %i\n", ast->boolean_operator);
    printf("boolean_value: %i\n", ast->boolean_value);

    printf("compound_length: %lu\n\n", ast->compound_size);

    printf("dict_key: %s\n\n", ast->dict_key);

    printf("math operator: %c\n", ast->math_expression_type);
    
    printf("####### END AST ANALYSIS #######\n");

    printf("\nProcess %i exited with code 1.\n", getpid());
    exit(1);
}
void error_exit(visitor_T* visitor, AST_T* ast)
{
    if (visitor->debug_mode == 1)
        AST_analysis(ast);
    else
        exit(1);
}