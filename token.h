#ifndef TOKEN_H
#define TOKEN_H
typedef struct TOKEN_STRUCT
{
    enum
    {
        TOKEN_ID,
        TOKEN_EQUALS,
        TOKEN_STRING,
        TOKEN_SEMI,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_RBRACE,
        TOKEN_LBRACE,
        TOKEN_COMMA,
        TOKEN_EOF,
        TOKEN_EQUALTO,
        TOKEN_NOTEQUALTO,
        TOKEN_GREATERTHAN,
        TOKEN_LESSTHAN,
        TOKEN_EGREATERTHAN,
        TOKEN_ELESSTHAN
    } type;

    char* value;
} token_T;

token_T* init_token(int type, char* value);
#endif
