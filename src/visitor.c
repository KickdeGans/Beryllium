#include "visitor.h"
#include "AST.h"
#include "scope.h"
#include "lib/io.h"
#include "compare.h"
#include "lexer.h"
#include "parser.h"
#include "import.h"
#include "thread.h"
#include "lib/http.h"
#include "builtin_functions.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/* Initiate visitor */
visitor_T* init_visitor()
{
    visitor_T* visitor = calloc(1, sizeof(struct VISITOR_STRUCT));
    visitor->scope = init_scope();
    visitor->prev_statement_value = 1;

    return visitor;
}

/* Visit Abstract Syntax Tree */
AST_T* visitor_visit(visitor_T* visitor, AST_T* node)
{
    if (node == (void*) 0)
    {
        return init_ast(AST_NOOP);
    }

    if (visitor == (void*) 0)
    {
        visitor = init_visitor();
    }

    if (node->is_return_value)
    {
        return node;
    }

    if (node->type != AST_STATEMENT_DEFINITION)
    {
        visitor->prev_statement_value = 1;
    }

    switch (node->type)
    {
        case AST_COMPOUND:                return visitor_visit_compound(visitor, node); break;
        case AST_VARIABLE_DEFINITION:     return visitor_visit_variable_definition(visitor, node); break;
        case AST_FUNCTION_DEFINITION:     return visitor_visit_function_definition(visitor, node); break;
        case AST_STATEMENT_DEFINITION:    return visitor_visit_statement_definition(visitor, node); break;
        case AST_VARIABLE_SETTER:         return visitor_visit_variable_setter(visitor, node); break;
        case AST_FUNCTION_RETURN:         return visitor_visit(visitor, node); break;
        case AST_FUNCTION_CALL:           return visitor_visit_function_call(visitor, node); break;
        case AST_STATEMENT_CALL:          return visitor_visit_statement_call(visitor, node); break;
        case AST_VARIABLE:                return visitor_visit_variable(visitor, node); break;
        case AST_STRING:                  return visitor_visit_string(visitor, node); break;
        case AST_INT:                     return visitor_visit_int(visitor, node); break;
        case AST_DOUBLE:                  return visitor_visit_double(visitor, node); break;
        case AST_BOOLEAN:                 return visitor_visit_boolean(visitor, node); break;
        case AST_STREAM:                  return visitor_visit_stream(visitor, node); break;
        case AST_ARRAY:                   return visitor_visit_array(visitor, node); break;
        case AST_GET_ARRAY_ITEM_BY_INDEX: return visitor_visit_get_array_by_index(visitor, node); break;
        case AST_DICT_ITEM:               return visitor_visit_dict_item(visitor, node); break;
        case AST_MATH_EXPR:               return visitor_visit_math_expr(visitor, node); break;
        case AST_ATTRIBUTE:               return visitor_visit_attribute(visitor, node); break;
        case AST_NOOP:                    return node; break;
        default: break;
    }
    
    printf("\nruntime error:\n    uncaught statement of type %d\n", node->type);
    exit(1);

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_attribute(visitor_T* visitor, AST_T* node)
{
    AST_T* value = visitor_visit(visitor, node->attribute_source);
    
    if (node->attribute_modifier->type == AST_FUNCTION_CALL)
    {
        //AST_T** args = node->attribute_modifier->function_call_arguments;
        //size_t args_size = node->attribute_modifier->function_call_arguments_size;
        
        if (strcmp(node->attribute_modifier->function_call_name, "toString") == 0)
        {
            AST_T* string = init_ast(AST_STRING);

            switch (value->type)
            {
                case AST_INT: sprintf(string->string_value, "%i", value->ast_int); break;
                case AST_STRING: string->string_value = value->string_value; break;
                default: break;
            }
            
            return string;
        }
    }

    printf("\nruntime error:\n    invalid attribute modifier type %i\n", node->attribute_modifier->type);
    exit(1);

    return init_ast(AST_NOOP);
}

/* Visit variable definition */
AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node)
{
    if (scope_get_variable_definition(node->scope, node->variable_definition_variable_name) != (void*) 0 || scope_get_variable_definition(visitor->scope, node->variable_definition_variable_name) != (void*) 0 || scope_get_variable_definition(node->private_scope, node->variable_definition_variable_name) != (void*) 0)
    {
        printf("\nruntime error:\n    variable with name %s already defined\n", node->variable_definition_variable_name);
        exit(1);
    }

    node->variable_definition_value = visitor_visit(visitor, node->variable_definition_value);

    if (node->variable_definition_value_type != node->variable_definition_value->type && node->variable_definition_value_type != AST_NOOP)
    {
        printf("\nruntime error:\n    invalid type for variable with name '%s'\n", node->variable_definition_variable_name);
        exit(1);
    }

    if (node->is_private)
    {
        scope_add_variable_definition(
            node->private_scope,
            node 
        );
        return node;
    }

    if (node->variable_definition_is_public)
    {
        scope_add_variable_definition(
            visitor->scope,
            node 
        );
        return node;
    }

    scope_add_variable_definition(
        node->scope,
        node 
    ); 

    return node;
}

/* Visit function definition */
AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node)
{
    scope_add_function_definition(
        visitor->scope,
        node
    );

    return node;
}

/* Visit variable */
AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node)
{
    AST_T* vdef = scope_get_variable_definition(
        node->private_scope,
        node->variable_name
    );
    
    if (vdef != (void*) 0)
        return vdef->variable_definition_value;

    vdef = scope_get_variable_definition(
        node->scope,
        node->variable_name
    );
    
    if (vdef != (void*) 0)
        return vdef->variable_definition_value;

    vdef = scope_get_variable_definition(
        visitor->scope,
        node->variable_name
    );

    if (vdef != (void*) 0)
        return vdef->variable_definition_value;

    printf("\nruntime error:\n    invalid variable definition with name '%s'\n", node->variable_name);
    exit(1);

    return init_ast(AST_NOOP);
}

/* Visit a statement definition */
AST_T* visitor_visit_statement_definition(visitor_T* visitor, AST_T* node)
{
    node->statement_definition_body->private_scope = node->private_scope;
    node->statement_definition_args[0]->private_scope = node->private_scope;

    if (strcmp(node->statement_definition_type, "if") == 0)
    {
        if (visitor_evaluate_boolean(visitor, visitor_visit(visitor, node->statement_definition_args[0])))
        {
            visitor->prev_statement_value = 1;
            return visitor_visit(visitor, node->statement_definition_body);
        }
        else
            visitor->prev_statement_value = 0;
    }
    if (strcmp(node->statement_definition_type, "else") == 0)
    {
        if (visitor->prev_statement_value == 0)
            return visitor_visit(visitor, node->statement_definition_body);
    }
    else if (strcmp(node->statement_definition_type, "while") == 0)
    {
        node->statement_definition_body->scope = node->scope;
        node->statement_definition_body->is_private = 1;
        while (visitor_evaluate_boolean(visitor, visitor_visit(visitor, node->statement_definition_args[0])))
        {
            if (visitor_visit(visitor, node->statement_definition_body)->break_)
                break;
            node->statement_definition_body->private_scope = (void*) 0;
        }
    }
    else if (strcmp(node->statement_definition_type, "until") == 0)
    {
        node->statement_definition_body->scope = node->scope;
        node->statement_definition_body->is_private = 1;
        while (visitor_evaluate_boolean(visitor, visitor_visit(visitor, node->statement_definition_args[0])))
        {
            if (visitor_visit(visitor, node->statement_definition_body)->break_)
                break;
            node->statement_definition_body->private_scope = (void*) 0;
        }
    }
    else if (strcmp(node->statement_definition_type, "dowhile") == 0)
    {
        node->statement_definition_body->scope = node->scope;
        node->statement_definition_body->is_private = 1;
        do 
        {
            if (visitor_visit(visitor, node->statement_definition_body)->break_)
                break;
            node->statement_definition_body->private_scope = (void*) 0;
        } while (visitor_evaluate_boolean(visitor, visitor_visit(visitor, node->statement_definition_args[0])));
    }
    else if (strcmp(node->statement_definition_type, "dountil") == 0)
    {
        node->statement_definition_body->scope = node->scope;
        node->statement_definition_body->is_private = 1;
        do 
        {
            if (visitor_visit(visitor, node->statement_definition_body)->break_)
                break;
            node->statement_definition_body->private_scope = (void*) 0;
        } while (visitor_evaluate_boolean(visitor, visitor_visit(visitor, node->statement_definition_args[0])));
    }
    else if (strcmp(node->statement_definition_type, "for") == 0)
    {
        visitor_visit_forloop(visitor, node);
    }

    return node;
}

/* Visit a forloop */
AST_T* visitor_visit_forloop(visitor_T* visitor, AST_T* node)
{
    AST_T* arguments = node->statement_definition_args[0];
    AST_T* value = visitor_visit(visitor, arguments->forloop_value);
    int variable_exists = 0;

    if (value->type == AST_ARRAY)
    {
        AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);

        variable_definition->variable_definition_variable_name = arguments->variable_definition_variable_name;
        variable_definition->variable_definition_value = init_ast(AST_NOOP);
        variable_definition->scope = node->scope;

        if (scope_get_variable_definition(node->scope, variable_definition->variable_definition_variable_name) == (void*) 0)
            scope_add_variable_definition(node->statement_definition_body->scope, variable_definition);
        else
            variable_exists = 1;

        scope_T* scope = node->statement_definition_body->scope;

        for (size_t i = 0; i < (int)value->array_size; i++)
        {
            scope_set_variable_definition(node->statement_definition_body->scope, value->array_value[i], variable_definition->variable_definition_variable_name);

            if (visitor_visit(visitor, node->statement_definition_body)->break_)
                break;

            node->statement_definition_body->scope = scope;
        }

        if (!variable_exists)
            scope_remove_variable_definition(node->scope, variable_definition->variable_definition_variable_name);

        return node;
    }
    if (value->type == AST_STRING)
    {
        AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);

        variable_definition->variable_definition_variable_name = arguments->variable_definition_variable_name;
        variable_definition->variable_definition_value = init_ast(AST_NOOP);
        variable_definition->scope = node->scope;

        if (scope_get_variable_definition(node->scope, variable_definition->variable_definition_variable_name) == (void*) 0)
            scope_add_variable_definition(node->statement_definition_body->scope, variable_definition);
        else
            variable_exists = 1;

        scope_T* scope = node->statement_definition_body->scope;

        for (size_t i = 0; i < strlen(value->string_value); i++)
        {
            AST_T* val = init_ast(AST_STRING);

            val->string_value = &value->string_value[i];

            scope_set_variable_definition(node->statement_definition_body->scope, val, variable_definition->variable_definition_variable_name);

            if (visitor_visit(visitor, node->statement_definition_body)->break_)
                break;

            node->statement_definition_body->scope = scope;
        }

        if (!variable_exists)
            scope_remove_variable_definition(node->scope, variable_definition->variable_definition_variable_name);

        return node;
    }
    if (value->type == AST_INT)
    {
        AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);

        variable_definition->variable_definition_variable_name = arguments->variable_definition_variable_name;
        variable_definition->variable_definition_value = init_ast(AST_NOOP);
        variable_definition->variable_definition_is_const = 0;
        variable_definition->scope = node->scope;

        if (scope_get_variable_definition(node->scope, variable_definition->variable_definition_variable_name) == (void*) 0)
            scope_add_variable_definition(node->statement_definition_body->scope, variable_definition);
        else
            variable_exists = 1;

        if (scope_get_variable_definition(node->private_scope, variable_definition->variable_definition_variable_name) == (void*) 0)
            scope_add_variable_definition(node->statement_definition_body->private_scope, variable_definition);
        else
            variable_exists = 1;

        scope_T* scope = node->statement_definition_body->scope;

        for (int i = 0; i < (int)value->ast_int; i++)
        {
            AST_T* item = init_ast(AST_INT);

            item->ast_int = i;

            scope_set_variable_definition(node->statement_definition_body->private_scope, item, variable_definition->variable_definition_variable_name);

            node->statement_definition_body->is_private = 1;
            AST_T* value = visitor_visit(visitor, node->statement_definition_body);

            if (value->break_)
                break;

            node->statement_definition_body->scope = scope;
        }

        free(scope);

        if (!variable_exists)
            scope_remove_variable_definition(node->scope, variable_definition->variable_definition_variable_name);

        return node;
    }
    return node;
}

/* Visit array get by index */
AST_T* visitor_visit_get_array_by_index(visitor_T* visitor, AST_T* node)
{
    node->array_item->private_scope = node->private_scope;
    node->array_index->private_scope = node->private_scope;
    AST_T* item = visitor_visit(visitor, node->array_item);
    AST_T* index = visitor_visit(visitor, node->array_index);
    if (item->type != AST_ARRAY)
    {
        printf("\nruntime error:\n    type must be array\n");
        exit(1);
    }
    if (index->type == AST_STRING)
    {
        for (int i = 0; i < item->array_size; i++)
        {
            if (strcmp(item->array_value[i]->dict_key, index->string_value) == 0)
            {
                return item->array_value[i]->dict_value;
            }
        }
        printf("\nruntime error:\n    key '%s' does not exist in dictionary\n", index->string_value);
        exit(1);
    }
    if (index->type != AST_INT)
    {
        printf("\nruntime error:\n    array index must be type 'int'\n");
        exit(1);
    }
    if (index->ast_int >= item->array_size)
    {
        printf("\nruntime error:\n    index was outside bounds of the array\n");
        exit(1);
    }
    return item->array_value[index->ast_int];
}

/* Visit a function call */
AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node)
{
    if (node->function_call_arguments != (void*) 0 && node->is_private)
    {
        for (int i = 0; i < node->function_call_arguments_size; i++)
            node->function_call_arguments[i]->private_scope = node->private_scope;
    }

    AST_T* func = try_run_builtin_function(visitor, node);

    if (func != (void*) 0)
        return func;

    AST_T* fdef = scope_get_function_definition(
        visitor->scope,
        node->function_call_name
    );
    
    if (fdef == (void*) 0)
    {
        printf("\nruntime error:\n    undefined method '%s'\n", node->function_call_name);
        exit(1);
    }
    

    if (node->function_call_arguments_size > 0 && fdef->function_definition_args_size > 0)
    {
        for (int i = 0; i < (int) node->function_call_arguments_size; i++)
        {
            AST_T* ast_var = (AST_T*) fdef->function_definition_args[i];
            AST_T* ast_value = (AST_T*) node->function_call_arguments[i];
            AST_T* ast_vardef = init_ast(AST_VARIABLE_DEFINITION);
            ast_vardef->variable_definition_value = visitor_visit(visitor, ast_value);
            ast_vardef->variable_definition_variable_name = (char*) calloc(strlen(ast_var->variable_name) + 1, sizeof(char));
            ast_vardef->variable_definition_is_weak = 1;
            strcpy(ast_vardef->variable_definition_variable_name, ast_var->variable_name);
            scope_add_variable_definition(fdef->function_definition_body->scope, ast_vardef);
        }
    }
    
    fdef->function_definition_body->is_private = 1;

    return visitor_visit(visitor, fdef->function_definition_body);
}

AST_T* visitor_visit_statement_call(visitor_T* visitor, AST_T* node)
{
    node->statement_call_argument->private_scope = node->private_scope;

    if (strcmp(node->statement_call_type, "return") == 0)
    {
        AST_T* ast = visitor_visit(visitor, node->statement_call_argument);
        ast->is_return_value = 1;
        return ast;
    }

    if (strcmp(node->statement_call_type, "include") == 0)
    {
        AST_T* ast = visitor_visit(visitor, node->statement_call_argument);
        import(visitor, ast->string_value);
        return ast;
    }

    if (strcmp(node->statement_call_type, "import") == 0)
    {
        AST_T* ast = visitor_visit(visitor, node->statement_call_argument);
        char path[255] = "/bin/fusion-lib/";
        strcat(path, ast->string_value);
        strcat(path, ".fn");
        import(visitor, path);
        return ast;
    }

    if (strcmp(node->statement_call_type, "break") == 0)
    {
        AST_T* ast = init_ast(AST_NOOP);
        ast->break_ = 1;
        return ast;
    }

    if (strcmp(node->statement_call_type, "continue") == 0)
    {
        AST_T* ast = init_ast(AST_NOOP);
        ast->continue_ = 1;
        return ast;
    }

    return node;
}

AST_T* visitor_visit_dict_item(visitor_T* visitor, AST_T* node)
{
    node->dict_value->private_scope = node->private_scope;
    node->dict_value = visitor_visit(visitor, node->dict_value);

    return node;
}

AST_T* visitor_visit_array(visitor_T* visitor, AST_T* node)
{
    AST_T* array = init_ast(AST_ARRAY);
    array->array_value = calloc(1, sizeof(struct AST_STRUCT*));
    node->array_value[0]->private_scope = node->private_scope;
    array->array_value[0] = visitor_visit(visitor, node->array_value[0]);
    array->array_size += 1;
    for (int i = 1; i < node->array_size; i++)
    {
        array->array_size += 1;
        array->array_value = realloc(
            array->array_value,
            array->array_size * sizeof(struct AST_STRUCT*)
        );
        node->array_value[i]->private_scope = node->private_scope;
        array->array_value[array->array_size-1] = visitor_visit(visitor, node->array_value[i]);
    }
    return array;
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_int(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_double(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_stream(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_boolean(visitor_T* visitor, AST_T* node)
{
    node->boolean_variable_a->private_scope = node->private_scope;
    node->boolean_variable_b->private_scope = node->private_scope;

    AST_T* value_a = visitor_visit(visitor, node->boolean_variable_a);
    AST_T* value_b = visitor_visit(visitor, node->boolean_variable_b);

    switch (node->boolean_operator)
    {
        case BOOLEAN_EQUALTO: node->boolean_value = compare(value_a, value_b); break;
        case BOOLEAN_NOTEQUALTO: node->boolean_value = !compare(value_a, value_b); break;
        case BOOLEAN_GREATERTHAN: 
            switch (value_a->type)
            {
                case AST_INT: node->boolean_value = value_a->ast_int > value_b->ast_int; break;
                case AST_DOUBLE: node->boolean_value = value_a->ast_double > value_b->ast_double; break;
                default: break;
            }
        break;
        case BOOLEAN_LESSTHAN: 
            switch (value_a->type)
            {
                case AST_INT: node->boolean_value = value_a->ast_int < value_b->ast_int; break;
                case AST_DOUBLE: node->boolean_value = value_a->ast_double < value_b->ast_double; break;
                default: break;
            }
        break;
        case BOOLEAN_EGREATERTHAN: 
            switch (value_a->type)
            {
                case AST_INT: node->boolean_value = value_a->ast_int >= value_b->ast_int; break;
                case AST_DOUBLE: node->boolean_value = value_a->ast_double >= value_b->ast_double; break;
                default: break;
            }
        break;
        case BOOLEAN_ELESSTHAN: 
            switch (value_a->type)
            {
                case AST_INT: node->boolean_value = value_a->ast_int <= value_b->ast_int; break;
                case AST_DOUBLE: node->boolean_value = value_a->ast_double <= value_b->ast_double; break;
                default: break;
            }
        break;
    }

    return node;
}

AST_T* visitor_visit_variable_setter(visitor_T* visitor, AST_T* node)
{
    node->variable_setter_value->private_scope = node->private_scope;
    AST_T* visited_ast = visitor_visit(visitor, node->variable_setter_value);

    if (scope_get_variable_definition(node->scope, node->variable_setter_variable_name) != (void*) 0)
    {
        if (scope_get_variable_definition(node->scope, node->variable_setter_variable_name)->variable_definition_value->type != visited_ast->type)
        {
            printf("runtime error:\n    variable type assignment error assigning variable '%s'\n", node->variable_setter_variable_name);
            exit(1);
        }
        scope_set_variable_definition(
            node->scope, 
            visited_ast, 
            node->variable_setter_variable_name
        );
    }
    else if (scope_get_variable_definition(node->private_scope, node->variable_setter_variable_name) != (void*) 0)
    {
        if (scope_get_variable_definition(node->private_scope, node->variable_setter_variable_name)->variable_definition_value->type != visited_ast->type)
        {
            printf("runtime error:\n    variable type assignment error assigning variable '%s'\n", node->variable_setter_variable_name);
            exit(1);
        }
        scope_set_variable_definition(
            node->private_scope, 
            visited_ast, 
            node->variable_setter_variable_name
        );
    }
    else if (scope_get_variable_definition(visitor->scope, node->variable_setter_variable_name) != (void*) 0)
    {
        if (scope_get_variable_definition(visitor->scope, node->variable_setter_variable_name)->variable_definition_value->type != visited_ast->type)
        {
            printf("runtime error:\n    variable type assignment error assigning variable '%s'\n", node->variable_setter_variable_name);
            exit(1);
        }
        scope_set_variable_definition(
            visitor->scope, 
            visited_ast, 
            node->variable_setter_variable_name
        );
    }

    else
    {
        printf("\nruntime error:\n    variable '%s' does not exist\n", node->variable_setter_variable_name);
        exit(1);
    }
    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_math_expr(visitor_T* visitor, AST_T* node)
{
    double value = 0;

    node->math_expression[0]->math_expression_value->private_scope = node->private_scope;
    AST_T* ast_value = visitor_visit(visitor, node->math_expression[0]->math_expression_value);
    
    switch (ast_value->type)
    {
        case AST_INT: value = ast_value->ast_int; break;
        case AST_DOUBLE: value = ast_value->ast_double; break;
        default: printf("\nruntime error:\n    can't interpret type %i as number", ast_value->type); exit(1);
    }

    for (int i = 1; i < node->math_expression_size; i++)
    {
        node->math_expression[i]->math_expression_value->private_scope = node->private_scope;
        
        AST_T* ast_value = visitor_visit(visitor, node->math_expression[i]->math_expression_value);
        double value_ = 0;

        switch (ast_value->type)
        {
            case AST_INT: value_ = ast_value->ast_int; break;
            case AST_DOUBLE: value_ = ast_value->ast_double; break;
            default: printf("\nruntime error:\n    can't interpret type %i as number", ast_value->type); exit(1);
        }
        
        switch (node->math_expression[i]->math_expression_type)
        {
            case '+': value += value_; break;
            case '-': value -= value_; break;
            case '/': value /= value_; break;
            case '*': value *= value_; break;
            case '%': value = (int)value % (int)value_; break;
        }
    }

    if ((int)value == value)
    {
        AST_T* ast = init_ast(AST_INT);

        ast->ast_int = value;
        ast->scope = node->scope;

        return ast;
    }

    AST_T* ast = init_ast(AST_DOUBLE);

    ast->ast_double = value;
    ast->scope = node->scope;

    return ast;
}

int visitor_evaluate_boolean(visitor_T* visitor, AST_T* node)
{
    switch (node->type)
    {
        case AST_BOOLEAN: return node->boolean_value; break;
        case AST_INT: return node->ast_int > 0; break;
        case AST_DOUBLE: return node->ast_double > 0; break;
        case AST_LONG: return node->ast_long > 0; break;
        default: return node->type != AST_NOOP; break;
    }

    return 0;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node)
{
    scope_T* private_scope = init_scope();

    for (int i = 0; i < node->compound_size; i++)
    {
        node->compound_value[i]->is_private = node->is_private;
        node->compound_value[i]->scope = node->scope;

        if (node->compound_value[i]->is_private)
        {
            node->compound_value[i]->private_scope = private_scope;
        }

        AST_T* v = visitor_visit(visitor, node->compound_value[i]);

        if (v->break_ || v->continue_ || v->is_return_value)
        {
            free(private_scope);

            return v;
        }
    }

    free(private_scope);

    return init_ast(AST_NOOP);
}