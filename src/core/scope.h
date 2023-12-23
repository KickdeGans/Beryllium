#ifndef SCOPE_H
#define SCOPE_H
#include "AST.h"

typedef struct SCOPE_STRUCT
{
    AST_T** function_definitions;
    size_t function_definitions_size;

    AST_T** variable_definitions;
    size_t variable_definitions_size;
} scope_T;

scope_T* init_scope(void);

AST_T* scope_add_function_definition(scope_T* scope, AST_T* fdef);

AST_T* scope_get_function_definition(scope_T* scope, const char* fname);

AST_T* scope_add_variable_definition(scope_T* scope, AST_T* vdef);

AST_T* scope_get_variable_definition(scope_T* scope, const char* name);

AST_T* scope_set_variable_definition(scope_T* scope, AST_T* vdef, const char* name);

AST_T* scope_remove_variable_definition(scope_T* scope, const char* name);

void scope_free(scope_T* scope);

void append_scope(scope_T* scope_dest, scope_T* scope_src);

#endif
