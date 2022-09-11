#include "lexer.h"
#include "name_verifier.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>


lexer_T* init_lexer(char* contents)
{
    lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer -> contents = contents;
    lexer -> i = 0;
    lexer -> prev_c = lexer->c;
    lexer -> c = contents[lexer -> i];

    return lexer;
}

void lexer_advance(lexer_T* lexer)
{
    if (lexer -> c != '\0' && lexer -> i < strlen(lexer -> contents))
    {
        lexer -> i += 1;
        lexer -> prev_c = lexer -> c;
        lexer -> c = lexer -> contents[lexer -> i];
    }
}

void lexer_skip_whitespace(lexer_T* lexer)
{
    while (lexer -> c == ' ' || lexer -> c == 10)
    {
        lexer_advance(lexer);
    }
}

void lexer_handle_comment(lexer_T* lexer)
{
    lexer_advance(lexer);
    while (lexer -> c != '#' || lexer -> c != 35)
    {
        lexer_advance(lexer);
    }
}

char lexer_next_token(lexer_T* lexer)
{
    return lexer->contents[lexer->i + 1];
}

token_T* lexer_get_next_token(lexer_T* lexer)
{
    while (lexer -> c != '\0' && lexer -> i < strlen(lexer -> contents))
    {
        if (lexer -> c == ' ' || lexer -> c == 10)
        {
            lexer_skip_whitespace(lexer);
        }
        if (lexer -> c == '#' || lexer -> c == 35)
        {
            lexer_handle_comment(lexer);
        }
        if (isalnum(lexer -> c))
        {
            return lexer_collect_id(lexer);
        }
        if (lexer -> c == '"')
        {
            return lexer_collect_string(lexer);
        }
        char next_token = lexer_next_token(lexer);
        switch (lexer -> c)
        {
            case '=': 
                switch (next_token)
                {
                    case '=': return lexer_advance_with_token(lexer, init_token(TOKEN_EQUALTO, lexer_get_current_char_as_string(lexer))); 
                    case '>': return lexer_advance_with_token(lexer, init_token(TOKEN_LAMBDA, lexer_get_current_char_as_string(lexer))); 
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
            case '>': 
                switch (next_token)
                {
                    case '=': return lexer_advance_with_token(lexer, init_token(TOKEN_GREATERTHAN, lexer_get_current_char_as_string(lexer))); 
                }
            return lexer_advance_with_token(lexer, init_token(TOKEN_GREATERTHAN, lexer_get_current_char_as_string(lexer))); break;
            case '<': 
                switch (next_token)
                {
                    case '=': return lexer_advance_with_token(lexer, init_token(TOKEN_ELESSTHAN, lexer_get_current_char_as_string(lexer))); 
                }
            return lexer_advance_with_token(lexer, init_token(TOKEN_LESSTHAN, lexer_get_current_char_as_string(lexer))); break;
            case '!': 
                switch (next_token)
                {
                    case '=': return lexer_advance_with_token(lexer, init_token(TOKEN_NOTEQUALTO, lexer_get_current_char_as_string(lexer))); 
                }
            return lexer_advance_with_token(lexer, init_token(TOKEN_EXCLAMATION, lexer_get_current_char_as_string(lexer))); break;
            case '?': return lexer_advance_with_token(lexer, init_token(TOKEN_QUESTION, lexer_get_current_char_as_string(lexer))); break;
        }
    }
    return init_token(TOKEN_EOF, "\0");
}

token_T* lexer_collect_string(lexer_T* lexer)
{
    lexer_advance(lexer);
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';
    while (lexer -> c != '"')
    {
        char* s = lexer_get_current_char_as_string(lexer);
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

    while (isalnum(lexer -> c))
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);

        lexer_advance(lexer);
    }

    if (name_verifier_is_valid_name(value) == 0)
    {
        printf("invalid identifier name '%s'\n", value);
        exit(1);
    }

    return init_token(TOKEN_ID, value);
}

token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token)
{
    lexer_advance(lexer);

    return token;
}

char* lexer_get_current_char_as_string(lexer_T* lexer)
{
    char* str = calloc(2, sizeof(char));
    str[0] = lexer -> c;
    str[1] = '\0';

    return str;
}
