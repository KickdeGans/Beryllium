#include "lexer.h"
#include "identifier.h"
#include "lib/string.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>


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

void lexer_advance(lexer_T* lexer)
{
    if (lexer->c != '\0' && lexer->i < strlen(lexer->contents))
    {
        lexer->i += 1;
        lexer->prev_c = lexer->c;
        lexer->c = lexer->contents[lexer->i];
    }
}

void lexer_skip_whitespace(lexer_T* lexer)
{
    while (lexer->c == ' ' || lexer->c == 10)
    {
        lexer_advance(lexer);
    }
}

void lexer_skip_comment(lexer_T* lexer)
{
    lexer_advance(lexer);
    while (lexer->c != '\n' || lexer-> c != EOF || lexer->c != '#' || lexer->c != '\0')
    {
        printf("%c", lexer->c);   
        lexer_advance(lexer);
    }
}

char lexer_next_token(lexer_T* lexer)
{
    return lexer->contents[lexer->i + 1];
}

token_T* lexer_get_next_token(lexer_T* lexer)
{
    while (lexer->c != '\0' && lexer->i < strlen(lexer->contents))
    {
        if (lexer->c == '\n')
        {
            lexer->current_line += 1;
        }
        if (lexer->c == ' ' || lexer->c == 10)
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
                    case '>': return lexer_advance_with_doubletok(lexer, init_token(TOKEN_LAMBDA, lexer_get_current_doubletok_as_string(lexer))); 
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
        }
    }
    return init_token(TOKEN_EOF, "\0");
}

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
            }
        }
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);

        lexer_advance(lexer);
    }
    lexer_advance(lexer);
    return init_token(TOKEN_STRING, value);
}

token_T* lexer_collect_id(lexer_T* lexer)
{
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';

    while (isalnum(lexer->c))
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);

        lexer_advance(lexer);
    }

    if (!is_valid_name(value) && !digits_only(value))
    {
        printf("\ncompilation error:\n    invalid identifier name '%s' at line %d\n", value, lexer->current_line);
        exit(1);
    }

    return init_token(TOKEN_ID, value);
}

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

token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token)
{
    lexer_advance(lexer);

    return token;
}

token_T* lexer_advance_with_doubletok(lexer_T* lexer, token_T* token)
{
    lexer_advance(lexer);
    lexer_advance(lexer);

    return token;
}

char* lexer_get_current_char_as_string(lexer_T* lexer)
{
    char* str = calloc(2, sizeof(char));
    str[0] = lexer->c;
    str[1] = '\0';
    return str;
}

char* lexer_get_current_doubletok_as_string(lexer_T* lexer)
{
    char* str = calloc(2, sizeof(char));
    str[0] = lexer->c;
    str[1] = lexer_next_token(lexer);

    return str;
}
