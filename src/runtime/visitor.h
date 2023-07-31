#ifndef VISITOR_H
#define VISITOR_H
#include "../core/AST.h"
#include "../core/scope.h"

typedef struct VISITOR_STRUCT
{
    struct SCOPE_STRUCT* scope;
    int prev_statement_value;
    char** args;
} visitor_T;

visitor_T* init_visitor();

AST_T* visitor_visit(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_attribute(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_statement_definition(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_forloop(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_foreach(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_array(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_get_array_by_index(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_statement_call(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_int(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_double(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_stream(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_boolean(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_variable_setter(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_dict_item(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_math_expr(visitor_T* visitor, AST_T* node);

int visitor_evaluate_boolean(visitor_T* visitor, AST_T* node);

#endif
