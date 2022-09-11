#ifndef NAME_VERIFIER_H
#define NAME_VERIFIER_H
#include <stdio.h>
#include <string.h>
#include "AST.h"

int name_verifier_is_valid_name(char* name);
int name_verifier_is_allowed_char(char ch);
int digits_only(const char *s);

#endif