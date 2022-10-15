#include "parser.h"
#include "AST.h"
#include "lexer.h"
#include "scope.h"
#include "lib/string.h"
#include "name_verifier.h"
#include "token.h"
#include <stdio.h>
#include <string.h>


parser_T* init_parser(lexer_T* lexer)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->prev_token = parser->current_token;
    parser->scope = init_scope();
    parser->eol = -1;
    return parser;
}

void parser_eat(parser_T* parser, int token_type)
{
    if (parser->current_token->type == token_type)
    {
        parser->prev_token = parser->current_token;
        parser->current_token = lexer_get_next_token(parser->lexer);
    }
    else if (token_type == TOKEN_SEMI)
    {
        printf("compilation error:\n    expected semicolon at line %d", parser->lexer->current_line);
    }
    else
    {
        printf("compilation error:\n    unexpected token '%s' at line %d. expected '%s'\n", parser->current_token->value, parser->lexer->current_line, token_get_token_name_from_type(token_type));
        exit(1);
    }
}

AST_T* parser_parse(parser_T* parser, scope_T* scope)
{
    return parser_parse_statements(parser, scope);
}

AST_T* parser_parse_statement(parser_T* parser, scope_T* scope)
{
    if (parser->current_token->type == TOKEN_ID)
    {
        return parser_parse_id(parser, scope);
    }
    return init_ast(AST_NOOP);
}

AST_T* parser_parse_statements(parser_T* parser, scope_T* scope)
{
    AST_T* compound = init_ast(AST_COMPOUND);
    compound->scope = scope;
    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));

    AST_T* ast_statement = parser_parse_statement(parser, scope);
    ast_statement->scope = scope;
    compound->compound_value[0] = ast_statement;
    compound->compound_size += 1;

    while (parser->current_token->type == TOKEN_SEMI)
    {
        parser_eat(parser, TOKEN_SEMI);

        AST_T* ast_statement = parser_parse_statement(parser, scope);

        if (ast_statement)
        {
            compound->compound_size += 1;
            compound->compound_value = realloc(
                compound->compound_value,
                compound->compound_size * sizeof(struct AST_STRUCT*)
            );
            compound->compound_value[compound->compound_size-1] = ast_statement;
        }
    }

    return compound;
}

AST_T* parser_parse_expr(parser_T* parser, scope_T* scope)
{
    while (parser->current_token->type != parser->eol)
    {
        switch (parser->current_token->type)
        { 
            case TOKEN_STRING: parser->prev_ast = parser_parse_string(parser, scope); break;
            case TOKEN_NUMBER: parser->prev_ast = parser_parse_number(parser, scope); break;
            case TOKEN_ID: parser->prev_ast = parser_parse_id(parser, scope); break;
            case TOKEN_EQUALTO: parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_NOTEQUALTO: parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_GREATERTHAN: parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_LESSTHAN: parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_EGREATERTHAN: parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_ELESSTHAN: parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_ARRPTR: parser->prev_ast = parser_parse_forloop(parser, scope);
            default: break;
        }
        if (parser->eol == -1)
        {
            break;
        }
    }
    parser->eol = -1;
    return parser->prev_ast;
}

AST_T* parser_parse_function_call(parser_T* parser, scope_T* scope)
{
    AST_T* function_call = init_ast(AST_FUNCTION_CALL);
    function_call->function_call_name = parser->prev_token->value;
    parser_eat(parser, TOKEN_LPAREN);
    function_call->function_call_arguments = calloc(1, sizeof(struct AST_STRUCT*));
    AST_T* ast_expr = parser_parse_expr(parser, scope);
    function_call->function_call_arguments[0] = ast_expr;
    function_call->function_call_arguments_size += 1;
    while (parser->current_token->type == TOKEN_COMMA)
    {
        parser_eat(parser, TOKEN_COMMA);
        AST_T* ast_expr = parser_parse_expr(parser, scope);
        function_call->function_call_arguments_size += 1;
        function_call->function_call_arguments = realloc(
            function_call->function_call_arguments,
            function_call->function_call_arguments_size * sizeof(struct AST_STRUCT*)
        );
        function_call->function_call_arguments[function_call->function_call_arguments_size-1] = ast_expr;
    }
    parser_eat(parser, TOKEN_RPAREN);
    function_call->scope = scope;
    return function_call;
}

AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope)
{
    parser_eat(parser, TOKEN_ID);
    char* variable_definition_variable_name = parser->current_token->value;
    if (name_verifier_is_valid_name(variable_definition_variable_name) == 0)
    {
        printf("compilation error:\n    invalid variable name '%s' at line %d", variable_definition_variable_name, parser->lexer->current_line);
        exit(1);
    }
    parser_eat(parser, TOKEN_ID);
    if (parser->current_token->type == TOKEN_SEMI)
    {
        AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);
        variable_definition->variable_definition_variable_name = variable_definition_variable_name;
        variable_definition->scope = scope;
        return variable_definition;
    }
    parser_eat(parser, TOKEN_EQUALS);
    AST_T* variable_definition_value = parser_parse_expr(parser, scope);
    AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);
    variable_definition->variable_definition_variable_name = variable_definition_variable_name;
    variable_definition->variable_definition_value = variable_definition_value;
    variable_definition->scope = scope;
    return variable_definition;
}
AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope)
{
    AST_T* ast = init_ast(AST_FUNCTION_DEFINITION);
    parser_eat(parser, TOKEN_ID);
    char* function_name = parser->current_token->value;
    ast->function_definition_name = calloc(
            strlen(function_name) + 1, sizeof(char)
    );
    strcpy(ast->function_definition_name, function_name);
    if (name_verifier_is_valid_name(function_name) == 0)
    {
        printf("compilation error:\n    invalid function name '%s' at line %d", function_name, parser->lexer->current_line);
        exit(1);
    }
    parser_eat(parser, TOKEN_ID);
    parser_eat(parser, TOKEN_LPAREN);
    ast->function_definition_args = calloc(1, sizeof(struct AST_STRUCT*));
    if (parser->current_token == TOKEN_ID)
    {
        AST_T* arg = parser_parse_variable(parser, scope);
        ast->function_definition_args_size += 1;
        ast->function_definition_args[ast->function_definition_args_size-1] = arg;
        while (parser->current_token->type == TOKEN_COMMA)
        {
            parser_eat(parser, TOKEN_COMMA);
            ast->function_definition_args_size += 1;
            ast->function_definition_args = realloc(ast->function_definition_args, ast->function_definition_args_size * sizeof(struct AST_STRUCT*));
            AST_T* arg = parser_parse_variable(parser, scope);
            ast->function_definition_args[ast->function_definition_args_size-1] = arg;
        }
    }
    parser_eat(parser, TOKEN_RPAREN);
    parser_eat(parser, TOKEN_LBRACE);
    ast->function_definition_body = parser_parse_statements(parser, scope);
    parser_eat(parser, TOKEN_RBRACE);
    ast->scope = scope;
    return ast;
}
AST_T* parser_parse_statement_definition(parser_T* parser, scope_T* scope)
{
    AST_T* ast = init_ast(AST_STATEMENT_DEFINITION);
    char* type = parser->current_token->value;
    ast->statement_definition_type = calloc(
            strlen(type) + 1, sizeof(char));
    parser_eat(parser, TOKEN_ID);
    strcpy(ast->statement_definition_type, type);
    if (strcmp(type, "else") == 0)
    {
        parser_eat(parser, TOKEN_LBRACE);
        ast->statement_definition_body = parser_parse_statements(parser, scope);
        parser_eat(parser, TOKEN_RBRACE);
        parser_eat(parser, TOKEN_SEMI);
        return ast;
    }
    parser_eat(parser, TOKEN_LPAREN);
    ast->statement_definition_args = calloc(1, sizeof(struct AST_STRUCT*));
    parser->eol = TOKEN_RPAREN;
    AST_T* arg = parser_parse_expr(parser, scope);
    ast->statement_definition_args_size += 1;
    ast->statement_definition_args[ast->statement_definition_args_size-1] = arg;
    parser_eat(parser, TOKEN_RPAREN);
    parser_eat(parser, TOKEN_LBRACE);
    ast->statement_definition_body = parser_parse_statements(parser, scope);
    parser_eat(parser, TOKEN_RBRACE);
    return ast;
}

AST_T* parser_parse_variable(parser_T* parser, scope_T* scope)
{
    char* token_value = parser->current_token->value;
    parser->prev_token = parser->current_token;
    parser_eat(parser, TOKEN_ID);
    if (parser->current_token->type == TOKEN_EQUALS)
    {
        return parser_parse_variable_setter(parser, scope);
    }
    if (parser->current_token->type == TOKEN_LPAREN)
    {
        return parser_parse_function_call(parser, scope);
    }

    AST_T* ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    ast_variable->scope = scope;

    return ast_variable;
}

AST_T* parser_parse_string(parser_T* parser, scope_T* scope)
{
    AST_T* ast_string = init_ast(AST_STRING);

    ast_string->string_value = parser->current_token->value;

    parser_eat(parser, TOKEN_STRING);

    ast_string->scope = scope;

    return ast_string;
}

AST_T* parser_parse_boolean(parser_T* parser, scope_T* scope)
{
    AST_T* ast_boolean = init_ast(AST_BOOLEAN);
    ast_boolean->boolean_variable_a = init_ast(AST_STRING);
    ast_boolean->boolean_variable_b = init_ast(AST_STRING);

    ast_boolean->boolean_variable_a = parser->prev_ast;

    switch (parser->current_token->type)
    {
        case TOKEN_EQUALTO: ast_boolean->boolean_operator = BOOLEAN_EQUALTO; parser_eat(parser, TOKEN_EQUALTO); break;
        case TOKEN_NOTEQUALTO: ast_boolean->boolean_operator = BOOLEAN_NOTEQUALTO; parser_eat(parser, TOKEN_NOTEQUALTO); break;
        case TOKEN_GREATERTHAN: ast_boolean->boolean_operator = BOOLEAN_GREATERTHAN; parser_eat(parser, TOKEN_GREATERTHAN); break;
        case TOKEN_LESSTHAN: ast_boolean->boolean_operator = BOOLEAN_LESSTHAN; parser_eat(parser, TOKEN_LESSTHAN); break;
        case TOKEN_EGREATERTHAN: ast_boolean->boolean_operator = BOOLEAN_EGREATERTHAN; parser_eat(parser, TOKEN_EGREATERTHAN); break;
        case TOKEN_ELESSTHAN: ast_boolean->boolean_operator = BOOLEAN_ELESSTHAN; parser_eat(parser, TOKEN_ELESSTHAN); break;
        default: printf("compilation error:\n    invalid boolean operator '%s' at line %d", parser->current_token->value, parser->lexer->current_line); exit(1);
    }
    
    ast_boolean->boolean_variable_b = parser_parse_expr(parser, scope);
    ast_boolean->scope = scope;

    return ast_boolean;
}

AST_T* parser_parse_number(parser_T* parser, scope_T* scope)
{
    AST_T* ast_number = init_ast(AST_NUMBER);

    ast_number->ast_number = strtod(parser->current_token->value, NULL);
    parser_eat(parser, TOKEN_NUMBER);

    ast_number->scope = scope;
    return ast_number;
}

AST_T* parser_parse_forloop(parser_T* parser, scope_T* scope)
{
    AST_T* ast_forloop = init_ast(AST_FORLOOP);

    ast_forloop->variable_definition_variable_name = parser->prev_ast->variable_name;

    parser_eat(parser, TOKEN_ARRPTR);

    ast_forloop->forloop_value = parser_parse_expr(parser, scope);

    ast_forloop->scope = scope;

    return ast_forloop;
}

AST_T* parser_parse_variable_setter(parser_T* parser, scope_T* scope)
{
    AST_T* ast_varset = init_ast(AST_VARIABLE_SETTTER);

    char* variable_setter_variable_name = parser->prev_token->value;
    ast_varset->variable_setter_variable_name = variable_setter_variable_name;

    parser_eat(parser, TOKEN_EQUALS);
    
    ast_varset->variable_setter_value = parser_parse_expr(parser, scope);

    ast_varset->scope = scope;
    return ast_varset;
}

AST_T* parser_parse_id(parser_T* parser, scope_T* scope)
{
    if (strcmp(parser->current_token->value, "var") == 0)
    {
        return parser_parse_variable_definition(parser, scope);
    }
    else if (strcmp(parser->current_token->value, "define") == 0)
    {
        return parser_parse_function_definition(parser, scope);
    }
    else if (strcmp(parser->current_token->value, "if") == 0 ||
        strcmp(parser->current_token->value, "else") == 0 ||
        strcmp(parser->current_token->value, "elseif") == 0 ||
        strcmp(parser->current_token->value, "while") == 0 ||
        strcmp(parser->current_token->value, "until") == 0 ||
        strcmp(parser->current_token->value, "dowhile") == 0 ||
        strcmp(parser->current_token->value, "dountil") == 0 ||
        strcmp(parser->current_token->value, "for") == 0)
    {
        return parser_parse_statement_definition(parser, scope);
    }
    else
    {
        return parser_parse_variable(parser, scope);
    }
}