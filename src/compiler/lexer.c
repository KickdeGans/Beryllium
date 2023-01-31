#include "lexer.h"
#include "identifier.h"
#include "../lib/string.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/* Initiate lexer */
lexer_T* init_lexer(char* contents)
{
    lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->contents = contents;
    lexer->i = 0;
    lexer->prev_c = lexer->c;
    lexer->c = contents[lexer->i];
    lexer->current_line = 1;

    return lexer;
}

/* Get character from file */
void lexer_advance(lexer_T* lexer)
{
    if (lexer->c != '\0' && lexer->i < strlen(lexer->contents))
    {
        lexer->i += 1;
        lexer->prev_c = lexer->c;
        lexer->c = lexer->contents[lexer->i];
    }
}

/* Skip whitespace */
/* Example */
/* Input: if (a == b) {} */
/* Output: if(a==b){} */
void lexer_skip_whitespace(lexer_T* lexer)
{
    while (lexer->c == ' ' || lexer->c == 10 || lexer->c == '	')
    {
        lexer_advance(lexer);
    }
}

/* Skip comment */
void lexer_skip_comment(lexer_T* lexer)
{
    while (lexer->c != '#')
    {
        lexer_advance(lexer);
    }
}

/* Get next char from file */
char lexer_next_token(lexer_T* lexer)
{
    return lexer->contents[lexer->i + 1];
}

/* Get next token */
/* Used by parser_eat() */
token_T* lexer_get_next_token(lexer_T* lexer)
{
    while (lexer->c != '\0' && lexer->i < strlen(lexer->contents))
    {
        if (lexer->c == '\n')
        {
            lexer->current_line += 1;
        }
        if (lexer->c == ' ' || lexer->c == 10 || lexer->c == '	')
        {
            lexer_skip_whitespace(lexer);
        }
        if (lexer->c == '#' || lexer->c == 35)
        {
            lexer_skip_comment(lexer);
        }
        if (isdigit(lexer->c))
        {
            return lexer_collect_number(lexer);
        }
        if (isalnum(lexer->c))
        {
            return lexer_collect_id(lexer);
        }
        if (lexer->c == '"')
        {
            return lexer_collect_string(lexer);
        }
        char next_token = lexer_next_token(lexer);
        switch (lexer->c)
        {
            case '=': 
                switch (next_token)
                {
                    case '=': return lexer_advance_with_doubletok(lexer, init_token(TOKEN_EQUALTO, lexer_get_current_doubletok_as_string(lexer))); 
                    case '>': return lexer_advance_with_doubletok(lexer, init_token(TOKEN_LAMDA, lexer_get_current_doubletok_as_string(lexer))); 
                }
            return lexer_advance_with_token(lexer, init_token(TOKEN_EQUALS, lexer_get_current_char_as_string(lexer))); break;
            case ';': return lexer_advance_with_token(lexer, init_token(TOKEN_SEMI, lexer_get_current_char_as_string(lexer))); break;
            case '(': return lexer_advance_with_token(lexer, init_token(TOKEN_LPAREN, lexer_get_current_char_as_string(lexer))); break;
            case ')': return lexer_advance_with_token(lexer, init_token(TOKEN_RPAREN, lexer_get_current_char_as_string(lexer))); break;
            case '{': return lexer_advance_with_token(lexer, init_token(TOKEN_LBRACE, lexer_get_current_char_as_string(lexer))); break;
            case '}': return lexer_advance_with_token(lexer, init_token(TOKEN_RBRACE, lexer_get_current_char_as_string(lexer))); break;
            case '[': return lexer_advance_with_token(lexer, init_token(TOKEN_LBRACKET, lexer_get_current_char_as_string(lexer))); break;
            case ']': return lexer_advance_with_token(lexer, init_token(TOKEN_RBRACKET, lexer_get_current_char_as_string(lexer))); break;
            case ',': return lexer_advance_with_token(lexer, init_token(TOKEN_COMMA, lexer_get_current_char_as_string(lexer))); break;
            case '.': return lexer_advance_with_token(lexer, init_token(TOKEN_DOT, lexer_get_current_char_as_string(lexer))); break;
            case ':': return lexer_advance_with_token(lexer, init_token(TOKEN_COLON, lexer_get_current_char_as_string(lexer))); break;
            case '>': 
                switch (next_token)
                {
                    case '=': return lexer_advance_with_doubletok(lexer, init_token(TOKEN_GREATERTHAN, lexer_get_current_doubletok_as_string(lexer))); 
                }
            return lexer_advance_with_token(lexer, init_token(TOKEN_GREATERTHAN, lexer_get_current_char_as_string(lexer))); break;
            case '<': 
                switch (next_token)
                {
                    case '=': return lexer_advance_with_doubletok(lexer, init_token(TOKEN_ELESSTHAN, lexer_get_current_doubletok_as_string(lexer))); 
                }
            return lexer_advance_with_token(lexer, init_token(TOKEN_LESSTHAN, lexer_get_current_char_as_string(lexer))); break;
            case '!': 
                switch (next_token)
                {
                    case '=': return lexer_advance_with_doubletok(lexer, init_token(TOKEN_NOTEQUALTO, lexer_get_current_doubletok_as_string(lexer))); 
                }
            return lexer_advance_with_token(lexer, init_token(TOKEN_EXCLAMATION, lexer_get_current_char_as_string(lexer))); break;
            case '?': return lexer_advance_with_token(lexer, init_token(TOKEN_QUESTION, lexer_get_current_char_as_string(lexer))); break;
            case '&':
                switch (next_token)
                {
                    case '&': return lexer_advance_with_doubletok(lexer, init_token(TOKEN_AND, lexer_get_current_doubletok_as_string(lexer))); 
                }
            case '|':
                switch (next_token)
                {
                    case '|': return lexer_advance_with_doubletok(lexer, init_token(TOKEN_OR, lexer_get_current_doubletok_as_string(lexer))); 
                }
            case '+': return lexer_advance_with_token(lexer, init_token(TOKEN_PLUS, lexer_get_current_char_as_string(lexer))); break;
            case '-': 
                switch (next_token)
                {
                    case '>': return lexer_advance_with_doubletok(lexer, init_token(TOKEN_POINT, lexer_get_current_doubletok_as_string(lexer))); 
                }
            return lexer_advance_with_token(lexer, init_token(TOKEN_MINUS, lexer_get_current_char_as_string(lexer))); break;
            case '/': return lexer_advance_with_token(lexer, init_token(TOKEN_SLASH, lexer_get_current_char_as_string(lexer))); break;
            case '*': return lexer_advance_with_token(lexer, init_token(TOKEN_STAR, lexer_get_current_char_as_string(lexer))); break;
            case '%': return lexer_advance_with_token(lexer, init_token(TOKEN_PERCENT, lexer_get_current_char_as_string(lexer))); break;
        }
    }
    return init_token(TOKEN_EOF, "\0");
}

/* Get string */
token_T* lexer_collect_string(lexer_T* lexer)
{
    lexer_advance(lexer);
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';
    while (lexer->c != '"')
    {
        char* s = lexer_get_current_char_as_string(lexer);
        if (strcmp(s, "\\") == 0)
        {
            lexer_advance(lexer);
            switch (lexer->c)
            {
                case 'b': s = "\b"; break;
                case 'f': s = "\f"; break;
                case 'n': s = "\n"; break;
                case 'r': s = "\r"; break;
                case '"': s = "\""; break;
                case 'a': s = "\a"; break;
                case 't': s = "\t"; break;
                case 'v': s = "\v"; break;
                case '0': s = "\0"; break;
                case '\\': s = "\\"; break;
            }
        }
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);

        lexer_advance(lexer);
    }
    lexer_advance(lexer);
    return init_token(TOKEN_STRING, value);
}

/* Get id */
/* Uses is_allowed_name() to check if the name is valid */
token_T* lexer_collect_id(lexer_T* lexer)
{
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';

    while (is_allowed_char(lexer->c))
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);

        lexer_advance(lexer);
    }

    if (strcmp(value, "end") == 0)
    {
        return init_token(TOKEN_END_KEYWORD, value);
    }

    if (!is_valid_name(value) && !digits_only(value))
    {
        printf("\ncompilation error:\n    invalid identifier name '%s' at line %d\n", value, lexer->current_line);
        exit(1);
    }

    return init_token(TOKEN_ID, value);
}

/* Get number */
token_T* lexer_collect_number(lexer_T* lexer)
{
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';
    while (isdigit(lexer->c) || lexer->c == '.')
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);

        lexer_advance(lexer);
    }
    return init_token(TOKEN_NUMBER, value);
}

/* Advance with token */
token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token)
{
    lexer_advance(lexer);

    return token;
}

/* Advance with token that has a length of 2 */
token_T* lexer_advance_with_doubletok(lexer_T* lexer, token_T* token)
{
    lexer_advance(lexer);
    lexer_advance(lexer);

    return token;
}

/* Get current char as string */
char* lexer_get_current_char_as_string(lexer_T* lexer)
{
    char* str = calloc(2, sizeof(char));
    str[0] = lexer->c;
    /* Use terminator at end of string */
    str[1] = '\0';
    return str;
}

/* Get current char[2] as string */
char* lexer_get_current_doubletok_as_string(lexer_T* lexer)
{
    char* str = calloc(3, sizeof(char));
    str[0] = lexer->c;
    str[1] = lexer_next_token(lexer);
    /* Use terminator at end of string */
    str[2] = '\0';

    return str;
}
