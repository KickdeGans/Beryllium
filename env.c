#include "visitor.h"
#include <stdio.h>

scope_T* load_env()
{
    scope_T* scope = init_scope();

    AST_T* variable = init_ast(AST_VARIABLE_DEFINITION);
    variable->variable_definition_is_const = 1;

    /* null */
    variable->variable_definition_variable_name = "null";
    variable->variable_definition_value = init_ast(AST_NOOP);
    scope_add_variable_definition(scope, variable);

    /* true */
    variable->variable_definition_variable_name = "true";
    variable->variable_definition_value = init_ast(AST_BOOLEAN);
    variable->variable_definition_value->boolean_value = 1;
    scope_add_variable_definition(scope, variable);

    /* false */
    variable->variable_definition_variable_name = "false";
    variable->variable_definition_value = init_ast(AST_BOOLEAN);
    variable->variable_definition_value->boolean_value = 0;
    scope_add_variable_definition(scope, variable);

    return scope;
}