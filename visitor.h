#ifndef VISITOR_H
#define VISITOR_H
#include "AST.h"
#include "scope.h"

typedef struct VISITOR_STRUCT
{
    struct SCOPE_STRUCT* scope;
    int prev_statement_value;
} visitor_T;

visitor_T* init_visitor();

AST_T* visitor_visit(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_statement_definition(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_forloop(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_array(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_get_array_by_index(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_boolean(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_variable_setter(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_dict_item(visitor_T* visitor, AST_T* node);

#endif
