#include "parser.h"
#include "../core/AST.h"
#include "lexer.h"
#include "../core/scope.h"
#include "../lib/string.h"
#include "../runtime/typename.h"
#include "identifier.h"
#include "token.h"
#include "../lib/debug.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Initiate parser */
parser_T* init_parser(lexer_T* lexer)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->scope = init_scope();
    parser->require_semicolon = 1;
    parser->has_parsed_root_compound = 0;
    parser->is_single_expression = 0;
    parser->in_field = (void*)0;

    return parser;
}

/* Get next token with type */
/* If desired token is not the next token the program exits with an error */
void parser_eat(parser_T* parser, int token_type)
{
    if (parser->current_token->type == token_type)
    {
        parser->prev_token = parser->current_token;
        parser->current_token = lexer_get_next_token(parser->lexer);
        //printf("[ '%s': '%s' ] ", token_get_token_name_from_type(parser->current_token->type), parser->current_token->value);
    }
    else
    {
        printf("\ncompilation error in %s:\n    unexpected token '%s' at line %d. expected '%s':\n         ", parser->in_field, parser->current_token->value, parser->lexer->current_line, token_get_token_name_from_type(token_type));
        
        int has_quote = 0;
        if (parser->prev_token->type == TOKEN_STRING)
        {
            printf("\"%s\"", parser->prev_token->value);
            has_quote = 1;
        }
        else 
            printf("%s", parser->prev_token->value);
        if (parser->current_token->type == TOKEN_STRING)
        {
            printf("\"%s\"", parser->current_token->value);
            has_quote = 1;
        }
        else 
            printf("%s", parser->current_token->value);
        if (lexer_get_next_token(parser->lexer)->type == TOKEN_STRING)
        {
            printf("\"%s\"\n", lexer_get_next_token(parser->lexer)->value);
            has_quote = 1;
        }
        else 
            printf("%s\n", lexer_get_next_token(parser->lexer)->value);

        if (has_quote)
            printf("           ");
        else
            printf("         ");

        for (size_t i = 0; i < strlen(parser->prev_token->value); i++)
        {
            printf(" ");
        }

        for (size_t i = 0; i < strlen(parser->current_token->value); i++)
        {
            printf("~");
        }  

        printf("\n");
        exit(1);
    }
}

/* Create Abstract Syntax Tree */
AST_T* parser_parse(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_root";

    return parser_parse_statements(parser, scope);
}

/* Parse a statement */
AST_T* parser_parse_statement(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_statement";

    if (parser->current_token->type == TOKEN_ID)
        return parser_parse_id(parser, scope);
    if (parser->current_token->type == TOKEN_DOT)
        return parser_parse_attribute(parser, scope);
    
    return init_ast(AST_NOOP);
}

/* Parse compound */
/* Usually within braces */
AST_T* parser_parse_statements(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_statements";

    AST_T* compound = init_ast(AST_COMPOUND);
    compound->scope = scope;
    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));
    if (!parser->has_parsed_root_compound)
    {
        parser->has_parsed_root_compound = 1;
        compound->is_root_compound = 1;
    }

    AST_T* ast_statement = parser_parse_statement(parser, scope);

    if (ast_statement->type == AST_NOOP)
        return init_ast(AST_NOOP);

    ast_statement->scope = scope;
    compound->compound_value[0] = ast_statement;
    compound->compound_size += 1;

    while (parser->current_token->type == TOKEN_SEMI || !parser->require_semicolon)
    {
        if (parser->require_semicolon)
            parser_eat(parser, TOKEN_SEMI);
        
        parser->require_semicolon = 1;

        AST_T* ast_statement = parser_parse_statement(parser, scope);

        if (ast_statement->type == AST_NOOP)
            return compound;

        compound->compound_size += 1;
        compound->compound_value = realloc(
            compound->compound_value,
            compound->compound_size * sizeof(struct AST_STRUCT*)
        );
        compound->compound_value[compound->compound_size-1] = ast_statement;

        if (parser->current_token->type == TOKEN_END)
            return compound;
    }

    return compound;
}

/* Parse an expression */
AST_T* parser_parse_expr(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_expr";

    while (1)   
    {
        if (parser->is_single_expression)
        {
            parser->is_single_expression = 0;

            switch (parser->current_token->type)
            {
                case TOKEN_STRING:       return parser_parse_string(parser, scope); break;
                case TOKEN_NUMBER:       return parser_parse_number(parser, scope); break;
                case TOKEN_ID:           return parser_parse_id(parser, scope); break;
                case TOKEN_LBRACE:       return parser_parse_array(parser, scope); break;
                case TOKEN_LBRACKET:     return parser_parse_array_get_by_index(parser, scope); break;
                case TOKEN_LPAREN:       return parser_parse_paren_expr(parser, scope); break;
                case TOKEN_COLON:        return parser_parse_dict_item(parser, scope); break;
                case TOKEN_DOT:          return parser_parse_attribute(parser, scope); break;

                case TOKEN_PLUS:         return parser_parse_math_expr(parser, scope); break;
                case TOKEN_MINUS:        return parser_parse_math_expr(parser, scope); break;
                case TOKEN_SLASH:        return parser_parse_math_expr(parser, scope); break;
                case TOKEN_STAR:         return parser_parse_math_expr(parser, scope); break;
                case TOKEN_PERCENT:      return parser_parse_math_expr(parser, scope); break;
                case TOKEN_PLUSEQUALS:   return parser_parse_inline_math_expr(parser, scope);
                case TOKEN_MINUSEQUALS:  return parser_parse_inline_math_expr(parser, scope);
                case TOKEN_STAREQUALS:   return parser_parse_inline_math_expr(parser, scope);
                case TOKEN_DIVEQUALS:    return parser_parse_inline_math_expr(parser, scope);
                case TOKEN_EQUALTO:      return parser_parse_boolean(parser, scope); break;
                case TOKEN_NOTEQUALTO:   return parser_parse_boolean(parser, scope); break;
                case TOKEN_GREATERTHAN:  return parser_parse_boolean(parser, scope); break;
                case TOKEN_LESSTHAN:     return parser_parse_boolean(parser, scope); break;
                case TOKEN_EGREATERTHAN: return parser_parse_boolean(parser, scope); break;
                case TOKEN_ELESSTHAN:    return parser_parse_boolean(parser, scope); break;

                default: break;
            }
        }

        switch (parser->current_token->type)
        {
            case TOKEN_STRING:       parser->prev_ast = parser_parse_string(parser, scope); break;
            case TOKEN_NUMBER:       parser->prev_ast = parser_parse_number(parser, scope); break;
            case TOKEN_ID:           parser->prev_ast = parser_parse_id(parser, scope); break;
            case TOKEN_LBRACE:       parser->prev_ast = parser_parse_array(parser, scope); break;
            case TOKEN_LBRACKET:     parser->prev_ast = parser_parse_array_get_by_index(parser, scope); break;
            case TOKEN_LPAREN:       parser->prev_ast = parser_parse_paren_expr(parser, scope); break;
            case TOKEN_COLON:        parser->prev_ast = parser_parse_dict_item(parser, scope); break;
            case TOKEN_DOT:          parser->prev_ast = parser_parse_attribute(parser, scope); break;

            case TOKEN_PLUS:         parser->prev_ast = parser_parse_math_expr(parser, scope); break;
            case TOKEN_MINUS:        parser->prev_ast = parser_parse_math_expr(parser, scope); break;
            case TOKEN_SLASH:        parser->prev_ast = parser_parse_math_expr(parser, scope); break;
            case TOKEN_STAR:         parser->prev_ast = parser_parse_math_expr(parser, scope); break;
            case TOKEN_PERCENT:      parser->prev_ast = parser_parse_math_expr(parser, scope); break;
            case TOKEN_EQUALTO:      parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_NOTEQUALTO:   parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_GREATERTHAN:  parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_LESSTHAN:     parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_EGREATERTHAN: parser->prev_ast = parser_parse_boolean(parser, scope); break;
            case TOKEN_ELESSTHAN:    parser->prev_ast = parser_parse_boolean(parser, scope); break;
            
            default: break;
        }

        if (parser->is_single_expression == 1 || 
            parser->current_token->type == TOKEN_RPAREN ||
            parser->current_token->type == TOKEN_RBRACE ||
            parser->current_token->type == TOKEN_COMMA ||
            parser->current_token->type == TOKEN_SEMI /*||
            parser->current_token->type == TOKEN_EQUALTO ||
            parser->current_token->type == TOKEN_NOTEQUALTO ||
            parser->current_token->type == TOKEN_GREATERTHAN ||
            parser->current_token->type == TOKEN_LESSTHAN ||
            parser->current_token->type == TOKEN_EGREATERTHAN ||
            parser->current_token->type == TOKEN_ELESSTHAN*/)
            break;
     }

    parser->is_single_expression = 0;

    AST_T* value = init_ast(AST_NOOP);
    *value = *parser->prev_ast;

    return value;
}

AST_T* parser_parse_attribute(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_attribute";

    AST_T* ast = init_ast(AST_ATTRIBUTE);

    ast->attribute_source = parser->prev_ast;

    while (parser->current_token->type == TOKEN_DOT)
    {
        ast->attribute_modifier_size++;

        parser_eat(parser, TOKEN_DOT);

        ast->attribute_modifier = realloc(
            ast->attribute_modifier,
            ast->attribute_modifier_size * sizeof(struct AST_STRUCT*)
        );
        
        parser->is_single_expression = 1;
        ast->attribute_modifier[ast->attribute_modifier_size-1] = parser_parse_expr(parser, scope);
    }

    return ast;
}

/* Parse an expression within parenthesis */
AST_T* parser_parse_paren_expr(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_paren_expresion";

    parser_eat(parser, TOKEN_LPAREN);

    AST_T* ast = parser_parse_expr(parser, scope);

    parser_eat(parser, TOKEN_RPAREN);
    
    return ast;
}

/* Parse a math expression */
/* Evaluated at runtime, not compile time */
AST_T* parser_parse_math_expr(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_math_expresion";

    AST_T* math_expr = init_ast(AST_MATH_EXPR);

    math_expr->math_expression = calloc(1, sizeof(struct AST_STRUCT*));
    math_expr->math_expression[0] = init_ast(AST_NOOP);
    math_expr->math_expression[0]->math_expression_value = parser->prev_ast;
    math_expr->math_expression_size += 1;
    
    while (parser->current_token->type == TOKEN_PLUS ||
           parser->current_token->type == TOKEN_MINUS ||
           parser->current_token->type == TOKEN_SLASH ||
           parser->current_token->type == TOKEN_STAR ||
           parser->current_token->type == TOKEN_PERCENT)
    {
        math_expr->math_expression_size += 1;

        math_expr->math_expression = realloc(
            math_expr->math_expression, 
            math_expr->math_expression_size * sizeof(struct AST_STRUCT*)
        );

        math_expr->math_expression[math_expr->math_expression_size-1] = init_ast(AST_NOOP);
        math_expr->math_expression[math_expr->math_expression_size-1]->math_expression_type = parser->current_token->value[0];

        switch (parser->current_token->type)
        {
            case TOKEN_PLUS: parser_eat(parser, TOKEN_PLUS); break;
            case TOKEN_MINUS: parser_eat(parser, TOKEN_MINUS); break;
            case TOKEN_SLASH: parser_eat(parser, TOKEN_SLASH); break;
            case TOKEN_STAR: parser_eat(parser, TOKEN_STAR); break;
            case TOKEN_PERCENT: parser_eat(parser, TOKEN_PERCENT); break;
            default: printf("compilation error:\n   unknown math operator '%s'", parser->current_token->value); exit(1); break;
        }

        parser->is_single_expression = 1;
        math_expr->math_expression[math_expr->math_expression_size-1]->math_expression_value = parser_parse_expr(parser, scope);
    }
    
    math_expr->scope = scope;
    return math_expr;
}


AST_T* parser_parse_inline_math_expr(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_inline_math_expr";

    AST_T* ast = init_ast(AST_INLINE_MATH_EXPR);

    ast->variable_name = parser->prev_token->value;

    switch (parser->current_token->type)
    {
        case TOKEN_PLUSEQUALS: ast->inline_math_expr_operator = '+'; parser_eat(parser, TOKEN_PLUSEQUALS);
        case TOKEN_MINUSEQUALS: ast->inline_math_expr_operator = '-'; parser_eat(parser, TOKEN_MINUSEQUALS);
        case TOKEN_DIVEQUALS: ast->inline_math_expr_operator = '/'; parser_eat(parser, TOKEN_DIVEQUALS);
        case TOKEN_STAREQUALS: ast->inline_math_expr_operator = '*'; parser_eat(parser, TOKEN_STAREQUALS);
        
        default: return init_ast(AST_NOOP);
    }

    parser->is_single_expression = 1;
    ast->inline_math_expr_value = parser_parse_expr(parser, scope);

    return ast;
}

/* Parse a function call */
AST_T* parser_parse_function_call(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_function_call";

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

/* Parse a statement call */
/* Example: */
/* return value; */
AST_T* parser_parse_statement_call(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_statement_call";

    AST_T* statement_call = init_ast(AST_STATEMENT_CALL);

    statement_call->statement_call_type = parser->current_token->value;
    
    parser_eat(parser, TOKEN_ID);

    if (fast_compare(statement_call->statement_call_type, "break") == 0 ||
        fast_compare(statement_call->statement_call_type, "continue") == 0)
    {
        return statement_call;
    }

    statement_call->statement_call_argument = parser_parse_expr(parser, scope);

    return statement_call;
}

/* Parse a variable definition */
AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_variable_definition";

    int is_public = 0;
    int is_const = 0;
    if (fast_compare(parser->current_token->value, "global") == 0)
    {
        is_public = 1;
        parser_eat(parser, TOKEN_ID);
    }
    if (fast_compare(parser->current_token->value, "val") == 0)
    {
        is_const = 1;
        parser_eat(parser, TOKEN_ID);
    }

    else if (fast_compare(parser->current_token->value, "var") == 0)
    {
        parser_eat(parser, TOKEN_ID);
    }
    
    char* variable_definition_variable_name = parser->current_token->value;
    if (!is_valid_name(variable_definition_variable_name))
    {
        printf("\ncompilation error:\n    invalid variable name '%s' at line %d\n", variable_definition_variable_name, parser->lexer->current_line);
        exit(1);
    }
    parser_eat(parser, TOKEN_ID);
    if (parser->current_token->type == TOKEN_SEMI)
    {
        AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);
        variable_definition->variable_definition_variable_name = variable_definition_variable_name;
        variable_definition->variable_definition_value = init_ast(AST_NOOP);
        variable_definition->scope = scope;
        return variable_definition;
    }
    parser_eat(parser, TOKEN_EQUALS);
    AST_T* variable_definition_value = parser_parse_expr(parser, scope);
    AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);
    variable_definition->variable_definition_variable_name = variable_definition_variable_name;
    variable_definition->variable_definition_is_public = is_public;
    variable_definition->variable_definition_is_const = is_const;
    variable_definition->variable_definition_value = variable_definition_value;
    variable_definition->scope = scope;
    return variable_definition;
}

/* Parse a function definition */
/* Example: */
/* 
fun hello() 
{
    puts("Hello world!\n");
}
*/
AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_function_definition";

    AST_T* ast = init_ast(AST_FUNCTION_DEFINITION);
    parser_eat(parser, TOKEN_ID);
    ast->function_definition_name = parser->current_token->value;
    if (!is_valid_name(ast->function_definition_name))
    {
        printf("\ncompilation error:\n    invalid function name '%s' at line %d\n", ast->function_definition_name, parser->lexer->current_line);
        exit(1);
    }
    parser_eat(parser, TOKEN_ID);
    parser_eat(parser, TOKEN_LPAREN);
    ast->function_definition_args = calloc(1, sizeof(struct AST_STRUCT*));
    if (parser->current_token->type == TOKEN_ID)
    {
        AST_T* arg = parser_parse_variable(parser, scope);
        ast->function_definition_args_size = 1;
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

    if (parser->current_token->type != TOKEN_LBRACE)
    {
        ast->function_definition_body = parser_parse_statement(parser, scope);
        ast->scope = scope;
        
        return ast;
    }

    parser_eat(parser, TOKEN_LBRACE);

    ast->function_definition_body = parser_parse_statements(parser, scope);

    parser->require_semicolon = 0;

    parser_eat(parser, TOKEN_RBRACE);
    
    return ast;
}

/* Parse a statement definition */
/* Example: */
/*
if (condition)
{
    doSomething();
}
*/
AST_T* parser_parse_statement_definition(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_statement_definition";

    AST_T* ast = init_ast(AST_STATEMENT_DEFINITION);
    char* type = parser->current_token->value;
    ast->statement_definition_type = calloc(
            strlen(type) + 1, sizeof(char));
    parser_eat(parser, TOKEN_ID);
    strcpy(ast->statement_definition_type, type);
    if (fast_compare(type, "else") == 0)
    {
        if (parser->current_token->type != TOKEN_LBRACE)
        {
            ast->statement_definition_body = parser_parse_statement(parser, scope);
            ast->scope = scope;

            return ast;
        }
        parser_eat(parser, TOKEN_LBRACE);
        ast->statement_definition_body = parser_parse_statements(parser, scope);
        parser_eat(parser, TOKEN_RBRACE);

        ast->scope = scope;
        
        return ast;
    }
    parser_eat(parser, TOKEN_LPAREN);
    ast->statement_definition_args = init_ast(AST_NOOP);

    if (fast_compare(ast->statement_definition_type, "for") == 0)
    {
        AST_T* arg = parser_parse_forloop(parser, scope);
        ast->statement_definition_args = arg;
    }
    else if (fast_compare(ast->statement_definition_type, "foreach") == 0)
    {
        AST_T* arg = parser_parse_foreach(parser, scope);
        ast->statement_definition_args = arg;
    }
    else
    {
        AST_T* arg = parser_parse_expr(parser, scope);
        ast->statement_definition_args = arg;
    }
    parser_eat(parser, TOKEN_RPAREN);
    if (parser->current_token->type != TOKEN_LBRACE)
    {
        ast->statement_definition_body = parser_parse_statement(parser, scope);
        ast->scope = scope;

        return ast;
    }
    parser_eat(parser, TOKEN_LBRACE);
    ast->statement_definition_body = parser_parse_statements(parser, scope);
    parser_eat(parser, TOKEN_RBRACE);

    parser->require_semicolon = 0;

    ast->scope = scope;
    return ast;
}

/* Parse a variable (identifier) */
AST_T* parser_parse_variable(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_variable";

    char* token_value = parser->current_token->value;
    parser->prev_token = parser->current_token;
    parser_eat(parser, TOKEN_ID);
    if (parser->current_token->type == TOKEN_DOT)
    {
        AST_T* ast = parser_parse_attribute(parser, scope);
        return ast;
    }
    if (parser->current_token->type == TOKEN_EQUALS)
    {
        return parser_parse_variable_setter(parser, scope);
    }
    if (parser->current_token->type == TOKEN_LPAREN)
    {
        return parser_parse_function_call(parser, scope);
    }
    if (parser->current_token->type == TOKEN_LBRACE)
    {
        parser_eat(parser, TOKEN_LBRACE);
        AST_T* ast = parser_parse_statements(parser, scope);
        parser_eat(parser, TOKEN_RBRACE);
        return ast;
    }
    if (parser->current_token->type == TOKEN_PLUSEQUALS ||
        parser->current_token->type == TOKEN_MINUSEQUALS ||
        parser->current_token->type == TOKEN_DIVEQUALS ||
        parser->current_token->type == TOKEN_STAREQUALS)
    {
        return parser_parse_inline_math_expr(parser, scope);
    }

    if (fast_compare(token_value, "int") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32000;
        ast->scope = scope;

        return ast;
    }
    else if (fast_compare(token_value, "double") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32001;
        ast->scope = scope;

        return ast;
    }
    else if (fast_compare(token_value, "bool") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32003;
        ast->scope = scope;

        return ast;
    }
    else if (fast_compare(token_value, "string") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32002;
        ast->scope = scope;

        return ast;
    }

    if (fast_compare(token_value, "true") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 1;

        return ast;
    }
    if (fast_compare(token_value, "false") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0;

        return ast;
    }
    if (fast_compare(token_value, "null") == 0)
    {
        AST_T* ast = init_ast(AST_NOOP);
        ast->scope = scope;

        return ast;
    }
    if (fast_compare(token_value, "eof") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = EOF;
        ast->scope = scope;

        return ast;
    }
    if (fast_compare(token_value, "void") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = -2;
        ast->scope = scope;

        return ast;
    }
    if (fast_compare(token_value, "array") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32004;
        ast->scope = scope;

        return ast;
    }
    if (fast_compare(token_value, "stream") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32005;
        ast->scope = scope;

        return ast;
    }

    AST_T* ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    ast_variable->scope = scope;

    return ast_variable;
}

/* Parse a string */
AST_T* parser_parse_string(parser_T* parser, scope_T* scope)
{
    //parser->is_single_expression = 1;
    parser->in_field = "parse_string";

    AST_T* ast_string = init_ast(AST_STRING);

    ast_string->string_value = parser->current_token->value;

    parser_eat(parser, TOKEN_STRING);

    return ast_string;
}

/* Parse an array */
AST_T* parser_parse_array(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_array";

    parser_eat(parser, TOKEN_LBRACE);
    AST_T* ast_array = init_ast(AST_ARRAY);
    ast_array->array_value = calloc(1, sizeof(struct AST_STRUCT*));
    ast_array->array_value[0] = parser_parse_expr(parser, scope);
    ast_array->array_size += 1;
    while (parser->current_token->type != TOKEN_RBRACE)
    {
        parser_eat(parser, TOKEN_COMMA);
        ast_array->array_size += 1;
        ast_array->array_value = realloc(
            ast_array->array_value,
            ast_array->array_size * sizeof(struct AST_STRUCT*)
        );
        ast_array->array_value[ast_array->array_size-1] = parser_parse_expr(parser, scope);
    }
    parser_eat(parser, TOKEN_RBRACE);
    ast_array->scope = scope;
    return ast_array;
}

/* Parse get by index */
/* Example: array[1] */
AST_T* parser_parse_array_get_by_index(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_array_get_by_index";

    AST_T* ast = init_ast(AST_GET_ARRAY_ITEM_BY_INDEX);

    ast->array_item = parser->prev_ast;

    parser_eat(parser, TOKEN_LBRACKET);

    ast->array_index = parser_parse_expr(parser, scope);

    parser_eat(parser, TOKEN_RBRACKET);

    ast->scope = scope; 

    return ast;
}

/* Parse a boolean expression */
/* Evaluated at runtime not compile time */
AST_T* parser_parse_boolean(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_boolean";

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
        default: printf("\ncompilation error:\n    invalid boolean operator '%s' at line %d\n", parser->current_token->value, parser->lexer->current_line); exit(1);
    }
    
    parser->is_single_expression = 1;
    ast_boolean->boolean_variable_b = parser_parse_expr(parser, scope);
    ast_boolean->scope = scope;
    parser->is_single_expression = 0;

    return ast_boolean;
}

/* Parse a number and determine its type by detecting decimals */
AST_T* parser_parse_number(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_number";

    if (((int)strtod(parser->current_token->value, NULL)) == strtod(parser->current_token->value, NULL))
    {
        AST_T* ast_number = init_ast(AST_INT);
        ast_number->ast_int = atoi(parser->current_token->value);
        parser_eat(parser, TOKEN_NUMBER);

        return ast_number;
    }
    else
    {
        AST_T* ast_number = init_ast(AST_DOUBLE);
        ast_number->ast_double = strtod(parser->current_token->value, NULL);
        parser_eat(parser, TOKEN_NUMBER);

        return ast_number;
    }

    return init_ast(AST_NOOP);
}

/* Parse a for loop expression */
AST_T* parser_parse_forloop(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_forloop";

    AST_T* ast_forloop = init_ast(AST_FORLOOP);

    ast_forloop->forloop_variable_definition = parser_parse_statement(parser, scope);

    parser_eat(parser, TOKEN_SEMI);

    parser->is_single_expression = 1;
    ast_forloop->forloop_condition = parser_parse_expr(parser, scope);

    parser_eat(parser, TOKEN_SEMI);

    ast_forloop->forloop_variable_modifier = parser_parse_statement(parser, scope);
    ast_forloop->forloop_variable_modifier->scope = scope;

    ast_forloop->scope = scope;

    return ast_forloop;
}

AST_T* parser_parse_foreach(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_foreach";

    AST_T* ast_foreach = init_ast(AST_FOREACH);

    ast_foreach->foreach_variable_name = parser->current_token->value;

    parser_eat(parser, TOKEN_ID);
    parser_eat(parser, TOKEN_IN);

    ast_foreach->foreach_source = parser_parse_expr(parser, scope);
    ast_foreach->foreach_source->scope = scope;

    ast_foreach->scope = scope;

    return ast_foreach;
}

/* Parse a variable setter */
/* Example: foo = bar; */
AST_T* parser_parse_variable_setter(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_variable_setter";

    AST_T* ast_varset = init_ast(AST_VARIABLE_SETTER);

    char* variable_setter_variable_name = parser->prev_token->value;
    ast_varset->variable_setter_variable_name = variable_setter_variable_name;

    parser_eat(parser, TOKEN_EQUALS);
    
    ast_varset->variable_setter_value = parser_parse_expr(parser, scope);

    ast_varset->scope = scope;
    return ast_varset;
}

/* Parse a dictionary item */
/* Example: "username": "user" */
AST_T* parser_parse_dict_item(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_dict_item";

    parser_eat(parser, TOKEN_COLON);
    AST_T* dict_item = init_ast(AST_DICT_ITEM);
    
    if (parser->prev_ast->type != AST_STRING)
    {
        printf("\ncompilation error:\n    dictionary key value must be of type string\n");
        exit(1);
    }

    dict_item->dict_key = parser->prev_ast->string_value;
    dict_item->dict_value = parser_parse_expr(parser, scope);
    dict_item->scope = scope;

    return dict_item;
}

/* Parse an identifier */
AST_T* parser_parse_id(parser_T* parser, scope_T* scope)
{
    parser->in_field = "parse_id";

    if (     fast_compare(parser->current_token->value, "var") == 0 ||
             fast_compare(parser->current_token->value, "val") == 0 ||
             fast_compare(parser->current_token->value, "global") == 0)
        return parser_parse_variable_definition(parser, scope);

    else if (fast_compare(parser->current_token->value, "fun") == 0)
        return parser_parse_function_definition(parser, scope);

    else if (fast_compare(parser->current_token->value, "if") == 0 ||
             fast_compare(parser->current_token->value, "else") == 0 ||
             fast_compare(parser->current_token->value, "while") == 0 ||
             fast_compare(parser->current_token->value, "for") == 0 ||
             fast_compare(parser->current_token->value, "foreach") == 0)
        return parser_parse_statement_definition(parser, scope);

    else if (fast_compare(parser->current_token->value, "return") == 0||
             fast_compare(parser->current_token->value, "break") == 0||
             fast_compare(parser->current_token->value, "continue") == 0||
             fast_compare(parser->current_token->value, "import") == 0||
             fast_compare(parser->current_token->value, "include") == 0)
        return parser_parse_statement_call(parser, scope);

    else
        return parser_parse_variable(parser, scope);
}