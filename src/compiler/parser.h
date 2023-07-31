#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "../core/AST.h"
#include "../core/scope.h"


typedef struct PARSER_STRUCT
{
    lexer_T* lexer;
    token_T* current_token;
    token_T* prev_token;
    scope_T* scope;
    AST_T* prev_ast;
    int require_semicolon;
    int has_parsed_root_compound;
} parser_T;

parser_T* init_parser(lexer_T* lexer);

void parser_eat(parser_T* parser, int token_type);

AST_T* parser_parse(parser_T* parser, scope_T* scope);

AST_T* parser_parse_statement(parser_T* parser, scope_T* scope);

AST_T* parser_parse_statements(parser_T* parser, scope_T* scope);

AST_T* parser_parse_expr(parser_T* parser, scope_T* scope);

AST_T* parser_parse_attribute(parser_T* parser, scope_T* scope);

AST_T* parser_parse_paren_expr(parser_T* parser, scope_T* scope);

AST_T* parser_parse_math_expr(parser_T* parser, scope_T* scope);

AST_T* parser_parse_function_call(parser_T* parser, scope_T* scope);

AST_T* parser_parse_statement_call(parser_T* parser, scope_T* scope);
        
AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope);

AST_T* parser_parse_statement_definition(parser_T* parser, scope_T* scope);

AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope);

AST_T* parser_parse_variable(parser_T* parser, scope_T* scope);

AST_T* parser_parse_string(parser_T* parser, scope_T* scope);

AST_T* parser_parse_array(parser_T* parser, scope_T* scope);

AST_T* parser_parse_array_get_by_index(parser_T* parser, scope_T* scope);

AST_T* parser_parse_boolean(parser_T* parser, scope_T* scope);

AST_T* parser_parse_number(parser_T* parser, scope_T* scope);

AST_T* parser_parse_forloop(parser_T* parser, scope_T* scope);

AST_T* parser_parse_foreach(parser_T* parser, scope_T* scope);

AST_T* parser_parse_variable_setter(parser_T* parser, scope_T* scope);

AST_T* parser_parse_dict_item(parser_T* parser, scope_T* scope);

AST_T* parser_parse_id(parser_T* parser, scope_T* scope);
#endif
