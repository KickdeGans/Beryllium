#include "token.h"
#include <stdlib.h>
#include <stdio.h>

/* Initiate a token */
token_T* init_token(int type, char* value)
{
    token_T* token = calloc(1, sizeof(struct TOKEN_STRUCT));
    token->type = type;
    token->value = value;
    
    return token;
}

/* Get token name and or value from type */
char* token_get_token_name_from_type(int type)
{
    switch (type)
    {
        case TOKEN_ID: return "identifier name";
        case TOKEN_EQUALS: return "=";
        case TOKEN_STRING: return "string value";
        case TOKEN_SEMI: return ";";
        case TOKEN_LPAREN: return "(";
        case TOKEN_RPAREN: return ")";
        case TOKEN_LBRACE: return "{";
        case TOKEN_RBRACE: return "}";
        case TOKEN_LBRACKET: return "[";
        case TOKEN_RBRACKET: return "]";
        case TOKEN_COMMA: return ",";
        case TOKEN_DOT: return ".";
        case TOKEN_EOF: return "end of file";
        case TOKEN_EQUALTO: return "==";
        case TOKEN_NOTEQUALTO: return "!=";
        case TOKEN_GREATERTHAN: return ">"; 
        case TOKEN_LESSTHAN: return "<";
        case TOKEN_EGREATERTHAN: return ">=";
        case TOKEN_ELESSTHAN: return "<=";
        case TOKEN_EXCLAMATION: return "!";
        case TOKEN_QUESTION: return "?";
        case TOKEN_LAMDA: return "=>";
        case TOKEN_AND: return "&&";
        case TOKEN_OR: return "||";
        case TOKEN_NUMBER: return "number value";
        case TOKEN_COLON: return ":";
    }
    return "";
}