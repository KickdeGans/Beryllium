#include "visitor.h"
#include "AST.h"
#include "scope.h"
#include "lib/io.h"
#include "array.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


visitor_T* init_visitor()
{
    visitor_T* visitor = calloc(1, sizeof(struct VISITOR_STRUCT));
    visitor->scope = (void*) 0;
    visitor->prev_statement_value = 1;

    return visitor;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* node)
{
    if (node->type != AST_STATEMENT_DEFINITION)
    {
        visitor->prev_statement_value = 1;
    }
    switch (node->type)
    {
        case AST_VARIABLE_DEFINITION: return visitor_visit_variable_definition(visitor, node); break;
        case AST_FUNCTION_DEFINITION: return visitor_visit_function_definition(visitor, node); break;
        case AST_STATEMENT_DEFINITION: return visitor_visit_statement_definition(visitor, node); break;
        case AST_VARIABLE: return visitor_visit_variable(visitor, node); break;
        case AST_FUNCTION_CALL: return visitor_visit_function_call(visitor, node); break;
        case AST_STRING: return visitor_visit_string(visitor, node); break;
        case AST_NUMBER: return visitor_visit_string(visitor, node); break;
        case AST_BOOLEAN: return visitor_visit_boolean(visitor, node); break;
        case AST_FUNCTION_RETURN: return visitor_visit(visitor, node); break;
        case AST_COMPOUND: return visitor_visit_compound(visitor, node); break;
        case AST_VARIABLE_SETTTER: return visitor_visit_variable_setter(visitor, node); break;
        case AST_NOOP: return node; break;
        default: return node; break;
    }
    printf("runtime error:\n    uncaught statement of type %d", node->type);
    exit(1);

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node)
{
    scope_add_variable_definition(
        node->scope,
        node        
    ); 

    return node;
}

AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node)
{
    scope_add_function_definition(
        node->scope,
        node        
    );

    return node;
}

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node)
{
    AST_T* vdef = scope_get_variable_definition(
        node->scope,
        node->variable_name
    );
    
    if (vdef != (void*) 0)
        return visitor_visit(visitor, vdef->variable_definition_value);

    printf("runtime error:\n    invalid variable definition with name '%s'", node->variable_name);
    exit(1);

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_statement_definition(visitor_T* visitor, AST_T* node)
{
    if (strcmp(node->statement_definition_type, "if") == 0)
    {
        if (visitor_visit_boolean(visitor, node->statement_definition_args[0])->boolean_value == 1)
        {
            visitor->prev_statement_value = 1;
            visitor_visit(visitor, node->statement_definition_body);
        }
        else
        {
            visitor->prev_statement_value = 0;
        }
    }
    if (strcmp(node->statement_definition_type, "else") == 0)
    {
        if (visitor->prev_statement_value == 0)
        {
            visitor_visit(visitor, node->statement_definition_body);
        }
    }
    if (strcmp(node->statement_definition_type, "elseif") == 0)
    {
        if (visitor_visit_boolean(visitor, node->statement_definition_args[0])->boolean_value == 1 && visitor->prev_statement_value == 0)
        {
            visitor->prev_statement_value = 1;
            visitor_visit(visitor, node->statement_definition_body);
        }
        else
        {
            visitor->prev_statement_value = 0;
        }
    }
    else if (strcmp(node->statement_definition_type, "while") == 0)
    {
        while (visitor_visit_boolean(visitor, node->statement_definition_args[0])->boolean_value == 1)
        {
            visitor_visit(visitor, node->statement_definition_body);
        }
    }
    else if (strcmp(node->statement_definition_type, "until") == 0)
    {
        while (visitor_visit_boolean(visitor, node->statement_definition_args[0])->boolean_value == 0)
        {
            visitor_visit(visitor, node->statement_definition_body);
        }
    }
    else if (strcmp(node->statement_definition_type, "dowhile") == 0)
    {
        do 
        {
            visitor_visit(visitor, node->statement_definition_body);
        } while (visitor_visit_boolean(visitor, node->statement_definition_args[0])->boolean_value == 1);
    }
    else if (strcmp(node->statement_definition_type, "dountil") == 0)
    {
        do 
        {
            visitor_visit(visitor, node->statement_definition_body);
        } while (visitor_visit_boolean(visitor, node->statement_definition_args[0])->boolean_value == 0);
    }
    else if (strcmp(node->statement_definition_type, "for") == 0)
    {
        visitor_visit_forloop(visitor, node);
    }

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_forloop(visitor_T* visitor, AST_T* node)
{
    AST_T* forloop_value = node->statement_definition_args[0];
    AST_T* value = visitor_visit(visitor, forloop_value->forloop_value);
    if (value->type == AST_ARRAY)
    {
        AST_T* vdef = init_ast(AST_VARIABLE_DEFINITION);
        vdef->variable_definition_variable_name = forloop_value->variable_definition_variable_name;
        vdef->variable_definition_value = value;
        vdef->scope = node->scope;
        scope_add_variable_definition(node->scope, vdef);
        for (int i = 0; i < (int)value->array_size; i++)
        {
            scope_set_variable_definition(node->scope, value->array_value[i], vdef->variable_definition_variable_name);
            visitor_visit(visitor, node->statement_definition_body);
        }
        return init_ast(AST_NOOP);
    }
    if (value->type == AST_NUMBER)
    {
        AST_T* vdef = init_ast(AST_VARIABLE_DEFINITION);
        vdef->variable_definition_variable_name = forloop_value->variable_definition_variable_name;
        vdef->variable_definition_value = value;
        vdef->scope = node->scope;
        scope_add_variable_definition(node->scope, vdef);
        for (int i = 0; i < (int)value->ast_number; i++)
        {
            AST_T* item = init_ast(AST_NUMBER);
            item->ast_number = i;
            scope_set_variable_definition(node->scope, item, vdef->variable_definition_variable_name);
            visitor_visit(visitor, node->statement_definition_body);
        }
        return init_ast(AST_NOOP);
    }
    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node)
{
    AST_T** args = node->function_call_arguments;
    size_t args_size = node->function_call_arguments_size;
    if (strcmp(node->function_call_name, "printfn") == 0)
    {
        for (int i = 0; i < args_size; i++)
        {
            AST_T* visited_ast = visitor_visit(visitor, args[i]);
            switch (visited_ast->type)
            {
                case AST_STRING: printf("%s", visited_ast->string_value); break;
                case AST_NUMBER: printf("%f", visited_ast->ast_number); break;
                default: printf("%s", visited_ast->string_value); break;
            }
        }
        return init_ast(AST_NOOP);
    }

    if (strcmp(node->function_call_name, "exit") == 0)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        exit((int) visited_ast->ast_number);
    }

    if (strcmp(node->function_call_name, "file.create") == 0)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        io_file_create(visited_ast->string_value);
        return init_ast(AST_NOOP);
    }
    
    if (strcmp(node->function_call_name, "file.delete") == 0)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        io_file_delete(visited_ast->string_value);
        return init_ast(AST_NOOP);
    }
    
    if (strcmp(node->function_call_name, "system") == 0)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        system(visited_ast->string_value);
        return init_ast(AST_NOOP);
    }

    if (strcmp(node->function_call_name, "return") == 0)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        visited_ast->type = AST_FUNCTION_RETURN;
        return visited_ast;
    }

    if (strcmp(node->function_call_name, "input") == 0)
    {
        AST_T* ast = init_ast(AST_STRING);
        for (int i = 0; i < args_size; i++)
        {
            AST_T* visited_ast = visitor_visit(visitor, args[i]);
            switch (visited_ast->type)
            {
                case AST_STRING: printf("%s", visited_ast->string_value); break;
                case AST_NUMBER: printf("%f", visited_ast->ast_number); break;
            }
        }
        scanf("%s", ast->string_value);
        return ast;
    }

    if (strcmp(node->function_call_name, "strtoi") == 0)
    {
        AST_T* visited_ast = init_ast(AST_NUMBER);
        visited_ast->ast_number = atoi(visitor_visit(visitor, args[0])->string_value);
        return visited_ast;
    }
    if (strcmp(node->function_call_name, "strtof") == 0)
    {
        AST_T* visited_ast = init_ast(AST_NUMBER);
        visited_ast->ast_number = atof(visitor_visit(visitor, args[0])->string_value);
        return visited_ast;
    }
    if (strcmp(node->function_call_name, "strtob") == 0)
    {
        AST_T* visited_ast = init_ast(AST_BOOLEAN);
        visited_ast->boolean_value = atoi(visitor_visit(visitor, args[0])->string_value);
        return visited_ast;
    }
    if (strcmp(node->function_call_name, "sizeof") == 0)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        visited_ast->ast_number = visited_ast->array_size;
        return visited_ast;
    }

    AST_T* fdef = scope_get_function_definition(
        node->scope,
        node->function_call_name
    );

    if (fdef == (void*)0)
    {
        printf("runtime error:\n    undefined method '%s'", node->function_call_name);
        exit(1);
    }
    if (node->function_call_arguments_size > 0 && fdef->function_call_arguments_size > 0)
    {
        for (int i = 0; i < (int) node->function_call_arguments_size; i++)
        {
            AST_T* ast_var = (AST_T*) fdef->function_definition_args[i];
            AST_T* ast_value = (AST_T*) node->function_call_arguments[i];
            AST_T* ast_vardef = init_ast(AST_VARIABLE_DEFINITION);
            ast_vardef->variable_definition_value = ast_value;
            ast_vardef->variable_definition_variable_name = (char*) calloc(strlen(ast_var->variable_name) + 1, sizeof(char));
            strcpy(ast_vardef->variable_definition_variable_name, ast_var->variable_name);
            scope_add_variable_definition(fdef->function_definition_body->scope, ast_vardef);
        }
    }
    
    return visitor_visit(visitor, fdef->function_definition_body);
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_boolean(visitor_T* visitor, AST_T* node)
{
    AST_T* value_a = visitor_visit(visitor, node->boolean_variable_a);
    AST_T* value_b = visitor_visit(visitor, node->boolean_variable_b);
    switch (node->boolean_operator)
    {
        case BOOLEAN_EQUALTO: node->boolean_value = value_a == value_b; break;
        case BOOLEAN_NOTEQUALTO: node->boolean_value = value_a != value_b; break;
        case BOOLEAN_GREATERTHAN: node->boolean_value = value_a->ast_number > value_b->ast_number; break;
        case BOOLEAN_LESSTHAN: node->boolean_value = value_a->ast_number < value_b->ast_number; break;
        case BOOLEAN_EGREATERTHAN: node->boolean_value = value_a->ast_number >= value_b->ast_number; break;
        case BOOLEAN_ELESSTHAN: node->boolean_value = value_a->ast_number <= value_b->ast_number; break;
    }
    return node;
}

AST_T* visitor_visit_variable_setter(visitor_T* visitor, AST_T* node)
{
    AST_T* visited_ast = visitor_visit(visitor, node->variable_setter_value);


    scope_set_variable_definition(
        node->scope, 
        visited_ast, 
        node->variable_setter_variable_name
    );

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node)
{
    for (int i = 0; i < node->compound_size; i++)
    {
        AST_T* v = visitor_visit(visitor, node->compound_value[i]);
        if (v->type == AST_FUNCTION_RETURN)
        {
            return v;
        }
    }

    return init_ast(AST_NOOP);
}