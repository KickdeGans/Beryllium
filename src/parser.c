#include "parser.h"
#include "AST.h"
#include "lexer.h"
#include "scope.h"
#include "lib/string.h"
#include "identifier.h"
#include "token.h"
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

    return parser;
}

/* Get next token with type */
/* If desired token is not the next token the program exists with an error */
void parser_eat(parser_T* parser, int token_type)
{
    if (parser->current_token->type == token_type)
    {
        parser->prev_token = parser->current_token;
        parser->current_token = lexer_get_next_token(parser->lexer);
    }
    else
    {
        printf("\ncompilation error:\n    unexpected token '%s' at line %d. expected '%s':\n         ", parser->current_token->value, parser->lexer->current_line, token_get_token_name_from_type(token_type));
        printf("%s%s%s\n         ", parser->prev_token->value, parser->current_token->value, lexer_get_next_token(parser->lexer)->value);

        for (int i = 0; i < strlen(parser->prev_token->value); i++)
        {
            printf(" ");
        }

        printf("^");

        for (int i = 0; i < strlen(parser->current_token->value) - 1; i++)
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
    return parser_parse_statements(parser, scope);
}

/* Parse a statement */
AST_T* parser_parse_statement(parser_T* parser, scope_T* scope)
{
    if (parser->current_token->type == TOKEN_ID)
    {
        return parser_parse_id(parser, scope);
    }
    return (void*) 0;
}

/* Parse compound */
/* Usually within braces */
AST_T* parser_parse_statements(parser_T* parser, scope_T* scope)
{
    AST_T* compound = init_ast(AST_COMPOUND);
    compound->scope = scope;
    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));

    AST_T* ast_statement = parser_parse_statement(parser, scope);

    if (ast_statement == (void*) 0)
    {
        return compound;
    }

    ast_statement->scope = scope;
    compound->compound_value[0] = ast_statement;
    compound->compound_size += 1;

    while (parser->current_token->type == TOKEN_SEMI || !parser->require_semicolon)
    {
        if (parser->require_semicolon)
        {
            parser_eat(parser, TOKEN_SEMI);
        }

        parser->require_semicolon = 1;

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

/* Parse an expression */
AST_T* parser_parse_expr(parser_T* parser, scope_T* scope)
{
    while (1)
    {
        if (parser->current_token->type == TOKEN_RBRACKET ||
            parser->current_token->type == TOKEN_RPAREN ||
            parser->current_token->type == TOKEN_RBRACE ||
            parser->current_token->type == TOKEN_COMMA ||
            parser->current_token->type == TOKEN_SEMI)
        {
            break;
        }
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
            case TOKEN_LBRACE: parser->prev_ast = parser_parse_array(parser, scope); break;
            case TOKEN_LBRACKET: parser->prev_ast = parser_parse_array_get_by_index(parser, scope); break;
            case TOKEN_LPAREN: parser->prev_ast = parser_parse_paren_expr(parser, scope); break;
            case TOKEN_COLON: parser->prev_ast = parser_parse_dict_item(parser, scope); break;
            case TOKEN_PLUS: parser->prev_ast = parser_parse_math_expr(parser, scope); break;
            case TOKEN_MINUS: parser->prev_ast = parser_parse_math_expr(parser, scope); break;
            case TOKEN_SLASH: parser->prev_ast = parser_parse_math_expr(parser, scope); break;
            case TOKEN_STAR: parser->prev_ast = parser_parse_math_expr(parser, scope); break;
            case TOKEN_PERCENT: parser->prev_ast = parser_parse_math_expr(parser, scope); break;
            case TOKEN_DOT: parser->prev_ast = parser_parse_attribute(parser, scope); break;
            default: break;
        }
    }
    return parser->prev_ast;
}

AST_T* parser_parse_attribute(parser_T* parser, scope_T* scope)
{
    AST_T* ast = init_ast(AST_ATTRIBUTE);

    ast->attribute_source = parser->prev_ast;

    parser_eat(parser, TOKEN_DOT);

    ast->attribute_modifier = parser_parse_expr(parser, scope);
    ast->scope = scope;

    return ast;
}

/* Parse an expression within parenthesis */
AST_T* parser_parse_paren_expr(parser_T* parser, scope_T* scope)
{
    parser_eat(parser, TOKEN_LPAREN);

    AST_T* ast = parser_parse_expr(parser, scope);

    parser_eat(parser, TOKEN_RPAREN);
    
    return ast;
}

/* Parse a math expression */
/* Evaluated at runtime, not compile time */
AST_T* parser_parse_math_expr(parser_T* parser, scope_T* scope)
{
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

        math_expr->math_expression[math_expr->math_expression_size-1]->math_expression_value = parser_parse_expr(parser, scope);
    }
    
    math_expr->scope = scope;
    return math_expr;
}

/* Parse a function call */
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

/* Parse a statement call */
/* Example: */
/* return value; */
AST_T* parser_parse_statement_call(parser_T* parser, scope_T* scope)
{
    AST_T* statement_call = init_ast(AST_STATEMENT_CALL);

    statement_call->statement_call_type = parser->current_token->value;
    
    parser_eat(parser, TOKEN_ID);

    if (strcmp(statement_call->statement_call_type, "break") == 0 ||
        strcmp(statement_call->statement_call_type, "continue") == 0)
    {
        return statement_call;
    }

    statement_call->statement_call_argument = parser_parse_expr(parser, scope);

    return statement_call;
}

/* Parse a variable definition */
/* Example: */
/* string name = "John" */
AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope)
{
    int is_public = 0;
    int is_const = 0;
    int type = AST_NOOP;
    if (strcmp(parser->current_token->value, "public") == 0)
    {
        is_public = 1;
        parser_eat(parser, TOKEN_ID);
    }
    if (strcmp(parser->current_token->value, "const") == 0)
    {
        is_const = 1;
        parser_eat(parser, TOKEN_ID);
    }
    if (strcmp(parser->current_token->value, "int") == 0)
    {
        type = AST_INT;
        parser_eat(parser, TOKEN_ID);
    }
    else if (strcmp(parser->current_token->value, "double") == 0)
    {
        type = AST_DOUBLE;
        parser_eat(parser, TOKEN_ID);
    }
    else if (strcmp(parser->current_token->value, "long") == 0)
    {
        type = AST_LONG;
        parser_eat(parser, TOKEN_ID);
    }
    else if (strcmp(parser->current_token->value, "bool") == 0)
    {
        type = AST_BOOLEAN;
        parser_eat(parser, TOKEN_ID);
    }
    else if (strcmp(parser->current_token->value, "string") == 0)
    {
        type = AST_STRING;
        parser_eat(parser, TOKEN_ID);
    }
    else if (strcmp(parser->current_token->value, "var") == 0)
    {
        type = AST_NOOP;
        parser_eat(parser, TOKEN_ID);
    }
    else if (strcmp(parser->current_token->value, "steam") == 0)
    {
        type = AST_STREAM;
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
    if (type == AST_NOOP)
        variable_definition->variable_definition_is_weak = 1;
    variable_definition->variable_definition_value_type = type;
    variable_definition->scope = scope;
    return variable_definition;
}

/* Parse a function definition */
/* Example: */
/* 
fn hello() 
{
    println("Hello world!");
} 
*/
AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope)
{
    AST_T* ast = init_ast(AST_FUNCTION_DEFINITION);
    parser_eat(parser, TOKEN_ID);
    char* function_name = parser->current_token->value;
    ast->function_definition_name = calloc(
            strlen(function_name) + 1, sizeof(char)
    );
    strcpy(ast->function_definition_name, function_name);
    if (!is_valid_name(function_name))
    {
        printf("\ncompilation error:\n    invalid function name '%s' at line %d\n", function_name, parser->lexer->current_line);
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
    }
    else
    {
        parser_eat(parser, TOKEN_LBRACE);
        ast->function_definition_body = parser_parse_statements(parser, scope);
        parser_eat(parser, TOKEN_RBRACE);

        parser->require_semicolon = 0;
    }
    
    ast->scope = scope;
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
    AST_T* ast = init_ast(AST_STATEMENT_DEFINITION);
    char* type = parser->current_token->value;
    ast->statement_definition_type = calloc(
            strlen(type) + 1, sizeof(char));
    parser_eat(parser, TOKEN_ID);
    strcpy(ast->statement_definition_type, type);
    if (strcmp(type, "else") == 0)
    {
        if (parser->current_token->type != TOKEN_LBRACE)
        {
            ast->statement_definition_body = parser_parse_statement(parser, scope);
        }
        else
        {
            parser_eat(parser, TOKEN_LBRACE);
            ast->statement_definition_body = parser_parse_statements(parser, scope);
            parser_eat(parser, TOKEN_RBRACE);
        }

        ast->scope = scope;
        
        return ast;
    }
    parser_eat(parser, TOKEN_LPAREN);
    ast->statement_definition_args = calloc(1, sizeof(struct AST_STRUCT*));

    if (strcmp(ast->statement_definition_type, "for") == 0)
    {
        AST_T* arg = parser_parse_forloop(parser, scope);
        ast->statement_definition_args_size += 1;
        ast->statement_definition_args[ast->statement_definition_args_size-1] = arg;
    }
    else
    {
        AST_T* arg = parser_parse_expr(parser, scope);
        ast->statement_definition_args_size += 1;
        ast->statement_definition_args[ast->statement_definition_args_size-1] = arg;
    }
    parser_eat(parser, TOKEN_RPAREN);
    if (parser->current_token->type != TOKEN_LBRACE)
    {
        ast->statement_definition_body = parser_parse_statement(parser, scope);
    }
    else
    {
        parser_eat(parser, TOKEN_LBRACE);
        ast->statement_definition_body = parser_parse_statements(parser, scope);
        parser_eat(parser, TOKEN_RBRACE);

        parser->require_semicolon = 0;
    }

    ast->scope = scope;
    return ast;
}

/* Parse struct definition (custom data type) */
/* Example: */
/* 
struct myType
{
    string name;
    int age;
}
*/
AST_T* parse_parse_struct_definition(parser_T* parser, scope_T* scope)
{
    AST_T* ast = init_ast(AST_STRUCT_DEFINITION);
    
    parser_eat(parser, TOKEN_ID);

    ast->struct_definition_name = parser->current_token->value;

    parser_eat(parser, TOKEN_ID);
    parser_eat(parser, TOKEN_LBRACE);

    ast->struct_definition_content = calloc(1, sizeof(struct AST_STRUCT*));
    ast->struct_definition_content_size += 1;
    ast->struct_definition_content[0] = parser_parse_variable_definition(parser, scope);

    while (parser->current_token->type == TOKEN_SEMI)
    {
        parser_eat(parser, TOKEN_SEMI);
        ast->struct_definition_content = realloc(
            ast->struct_definition_content, 
            ast->struct_definition_content_size * sizeof(struct AST_STRUCT*)
        );
        ast->struct_definition_content_size += 1;
        ast->struct_definition_content[ast->struct_definition_content_size-1] = parser_parse_variable_definition(parser, scope);
    }

    parser_eat(parser, TOKEN_RBRACE);

    return ast;
}

/* Parse a variable (identifier) */
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
    if (parser->current_token->type == TOKEN_LBRACE)
    {
        parser_eat(parser, TOKEN_LBRACE);
        AST_T* ast = parser_parse_statements(parser, scope);
        parser_eat(parser, TOKEN_RBRACE);
        return ast;
    }

    if (strcmp(token_value, "true") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 1;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "false") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "null") == 0)
    {
        AST_T* ast = init_ast(AST_NOOP);
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "stdout") == 0)
    {
        AST_T* ast = init_ast(AST_STREAM);
        ast->stream = stdout;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "stdin") == 0)
    {
        AST_T* ast = init_ast(AST_STREAM);
        ast->stream = stdin;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "eof") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = EOF;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "void") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = -2;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "int32") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32000;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "int64") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32006;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "double32") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32001;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "str") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32002;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "int1") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32003;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "arr") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32004;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "stream") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32005;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "char") == 0)
    {
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = 0x32006;
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "fin") == 0)
    {
        AST_T* ast = init_ast(AST_STRING);
        ast->string_value = "rb";
        ast->scope = scope;

        return ast;
    }
    if (strcmp(token_value, "fout") == 0)
    {
        AST_T* ast = init_ast(AST_STRING);
        ast->string_value = "w";
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
    AST_T* ast_string = init_ast(AST_STRING);

    ast_string->string_value = parser->current_token->value;

    parser_eat(parser, TOKEN_STRING);

    ast_string->scope = scope;

    return ast_string;
}

/* Parse an array */
AST_T* parser_parse_array(parser_T* parser, scope_T* scope)
{
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
    
    ast_boolean->boolean_variable_b = parser_parse_expr(parser, scope);
    ast_boolean->scope = scope;

    return ast_boolean;
}

/* Parse a number and determine its type by detecting decimals */
AST_T* parser_parse_number(parser_T* parser, scope_T* scope)
{
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
    AST_T* ast_forloop = init_ast(AST_FORLOOP);

    ast_forloop->variable_definition_variable_name = parser->current_token->value;

    parser_eat(parser, TOKEN_ID);

    parser_eat(parser, TOKEN_COLON);

    ast_forloop->forloop_value = parser_parse_expr(parser, scope);

    ast_forloop->scope = scope;

    return ast_forloop;
}

/* Parse a variable setter */
/* Example: foo = bar; */
AST_T* parser_parse_variable_setter(parser_T* parser, scope_T* scope)
{
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
    if (strcmp(parser->current_token->value, "var") == 0 ||
        strcmp(parser->current_token->value, "string") == 0 ||
        strcmp(parser->current_token->value, "int") == 0 ||
        strcmp(parser->current_token->value, "double") == 0 ||
        strcmp(parser->current_token->value, "long") == 0 ||
        strcmp(parser->current_token->value, "size") == 0 ||
        strcmp(parser->current_token->value, "char") == 0 ||
        strcmp(parser->current_token->value, "bool") == 0 ||
        strcmp(parser->current_token->value, "stream") == 0 ||
        strcmp(parser->current_token->value, "public") == 0 ||
        strcmp(parser->current_token->value, "const") == 0)
    {
        return parser_parse_variable_definition(parser, scope);
    }
    else if (strcmp(parser->current_token->value, "proc") == 0)
    {
        return parser_parse_function_definition(parser, scope);
    }
    else if (strcmp(parser->current_token->value, "if") == 0 ||
             strcmp(parser->current_token->value, "else") == 0 ||
             strcmp(parser->current_token->value, "while") == 0 ||
             strcmp(parser->current_token->value, "until") == 0 ||
             strcmp(parser->current_token->value, "dowhile") == 0 ||
             strcmp(parser->current_token->value, "dountil") == 0 ||
             strcmp(parser->current_token->value, "for") == 0)
    {
        return parser_parse_statement_definition(parser, scope);
    }
    else if (strcmp(parser->current_token->value, "return") == 0||
             strcmp(parser->current_token->value, "break") == 0||
             strcmp(parser->current_token->value, "continue") == 0||
             strcmp(parser->current_token->value, "import") == 0||
             strcmp(parser->current_token->value, "include") == 0)
    {
        return parser_parse_statement_call(parser, scope);
    }
    else if (strcmp(parser->current_token->value, "struct") == 0)
    {
        return parse_parse_struct_definition(parser, scope);
    }
    else
    {
        return parser_parse_variable(parser, scope);
    }
}