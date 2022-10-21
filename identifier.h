#ifndef IDENTIFIER_H
#define IDENTIFIER_H
#include <stdio.h>
#include <string.h>
#include "AST.h"

int is_valid_name(char* name);
int is_allowed_char(char ch);
int digits_only(const char *s);

#endif