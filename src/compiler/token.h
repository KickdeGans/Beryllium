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
        TOKEN_LBRACKET,
        TOKEN_RBRACKET,
        TOKEN_COMMA,
        TOKEN_DOT,
        TOKEN_EOF,
        TOKEN_EQUALTO,
        TOKEN_NOTEQUALTO,
        TOKEN_GREATERTHAN,
        TOKEN_LESSTHAN,
        TOKEN_EGREATERTHAN,
        TOKEN_ELESSTHAN,
        TOKEN_EXCLAMATION,
        TOKEN_QUESTION,
        TOKEN_LAMDA,
        TOKEN_AND,
        TOKEN_OR,
        TOKEN_NUMBER,
        TOKEN_COLON,
        TOKEN_PLUS,
        TOKEN_MINUS,
        TOKEN_SLASH,
        TOKEN_STAR,
        TOKEN_PERCENT,
        TOKEN_POINT,
        TOKEN_DOUBLE_PLUS,
        TOKEN_DOUBLE_MINUS,
        TOKEN_IN,
        TOKEN_END,
        TOKEN_PLUSEQUALS,
        TOKEN_MINUSEQUALS,
        TOKEN_STAREQUALS,
        TOKEN_DIVEQUALS
    } type;

    char* value;
} token_T;

token_T* init_token(int type, char* value);
char* token_get_token_name_from_type(int type);

#endif
