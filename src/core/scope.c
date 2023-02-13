#include "AST.h"
#include "scope.h"
#include <string.h>
#include <stdio.h>

/* Initiate the scope */
scope_T* init_scope()
{
    scope_T* scope = calloc(1, sizeof(struct SCOPE_STRUCT));

    scope->function_definitions = (void*) 0;
    scope->function_definitions_size = 0;

    scope->variable_definitions = (void*) 0;
    scope->variable_definitions_size = 0;

    return scope;
}

/* Add a function definition */
AST_T* scope_add_function_definition(scope_T* scope, AST_T* fdef)
{
    scope->function_definitions_size += 1;

    if (scope->function_definitions == (void*)0)
    {
        scope->function_definitions = calloc(1, sizeof(struct AST_STRUCT*));
    }
    else
    {
        scope->function_definitions =
            realloc(
                scope->function_definitions,
                scope->function_definitions_size * sizeof(struct AST_STRUCT**)
            );
    }

    scope->function_definitions[scope->function_definitions_size-1] =
        fdef;

    return fdef;
}

/* Get a function definition */
AST_T* scope_get_function_definition(scope_T* scope, const char* fname)
{
    if (scope == (void*) 0)
    {
        return (void*) 0;
    }

    for (int i = 0; i < scope->function_definitions_size; i++)
    {
        AST_T* fdef = scope->function_definitions[i];

        if (strcmp(fdef->function_definition_name, fname) == 0)
        {
            return fdef;
        }
    }

    return (void*)0;
}

/* Add a variable definition */
AST_T* scope_add_variable_definition(scope_T* scope, AST_T* vdef)
{
    if (scope == (void*) 0)
    {
        scope = init_scope();
    }

    if (scope->variable_definitions == (void*) 0)
    {
        scope->variable_definitions = calloc(1, sizeof(struct AST_STRUCT*));
        scope->variable_definitions[0] = vdef;
        scope->variable_definitions_size += 1;
    }
    else
    {
        scope->variable_definitions_size += 1;
        scope->variable_definitions = realloc(
            scope->variable_definitions,
            scope->variable_definitions_size * sizeof(struct AST_STRUCT*)
        );
        scope->variable_definitions[scope->variable_definitions_size-1] = vdef;
    }
    return vdef;
}

/* Get a variable definition */
AST_T* scope_get_variable_definition(scope_T* scope, const char* name)
{
    if (scope == (void*) 0)
    {
        return (void*) 0;
    }

    for (int i = 0; i < scope->variable_definitions_size; i++)
    {
        AST_T* vdef = scope->variable_definitions[i];

        if (strcmp(vdef->variable_definition_variable_name, name) == 0)
        {
            return vdef;
        }
    }

    return (void*)0;
}

/* Change the value of a variable */
AST_T* scope_set_variable_definition(scope_T* scope, AST_T* vdef, const char* name)
{
    if (scope == (void*) 0)
    {
        return (void*) 0;
    }
    if (vdef == (void*) 0)
    {
        return (void*) 0;
    }
    if (name == (void*) 0)
    {
        return (void*) 0;
    }

    for (int i = 0; i < scope->variable_definitions_size; i++)
    {
        AST_T* vdef_ = scope->variable_definitions[i];
        
        if (strcmp(vdef_->variable_definition_variable_name, name) == 0)
        {
            if (scope->variable_definitions[i]->variable_definition_is_const)
            {
                printf("\nruntime error:\n    can't re-assign value to constant '%s'\n", name);
                exit(1);
            }
            scope->variable_definitions[i]->variable_definition_value = vdef;
            return vdef;
        }
    }
    return (void*) 0;
}

/* Remove a variable definition */
/* Used by the free() function */
AST_T* scope_remove_variable_definition(scope_T* scope, const char* name)
{
    if (scope == (void*) 0)
    {
        return (void*) 0;
    }

    for (int i = 0; i < scope->variable_definitions_size; i++)
    {
        AST_T* vdef_ = scope->variable_definitions[i];
        
        if (strcmp(vdef_->variable_definition_variable_name, name) == 0)
        {
            scope->variable_definitions[i] = (void*) 0;
            break;
        }
    }
    return (void*)0;
}

void scope_free(scope_T* scope)
{
    scope = NULL;

    free(scope);

    return;
}