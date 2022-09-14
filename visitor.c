#include "visitor.h"
#include "AST.h"
#include "scope.h"
#include "exception.h"
#include "lib/io.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

visitor_T* init_visitor()
{
    visitor_T* visitor = calloc(1, sizeof(struct VISITOR_STRUCT));

    return visitor;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* node)
{
    switch (node->type)
    {
        case AST_VARIABLE_DEFINITION: return visitor_visit_variable_definition(visitor, node); break;
        case AST_FUNCTION_DEFINITION: return visitor_visit_function_definition(visitor, node); break;
        case AST_STATEMENT_DEFINITION: return visitor_visit_statement_definition(visitor, node); break;
        case AST_VARIABLE: return visitor_visit_variable(visitor, node); break;
        case AST_FUNCTION_CALL: return visitor_visit_function_call(visitor, node); break;
        case AST_STRING: return visitor_visit_string(visitor, node); break;
        case AST_FUNCTION_RETURN: return visitor_visit(visitor, node); break;
        case AST_COMPOUND: return visitor_visit_compound(visitor, node); break;
        case AST_NOOP: return node; break;
    }
    throw_exception("uncaught statement of type", (char*)node->type);

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

    throw_exception("invalid variable definition", "");
    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_statement_definition(visitor_T* visitor, AST_T* node)
{
    if (strcmp(node->statement_definition_type, "if") == 0)
    {
        if (visitor_visit_boolean(visitor, node)->boolean_value == 1)
        {
            visitor_visit(visitor, node->statement_definition_body);
        }
    }
    else if (strcmp(node->statement_definition_type, "while") == 0)
    {
        while (visitor_visit_boolean(visitor, node)->boolean_value == 1)
        {
            visitor_visit(visitor, node->statement_definition_body);
        }
    }
    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node)
{
    AST_T** args = node->function_call_arguments;
    int args_size = node->function_call_arguments_size;
    if (strcmp(node->function_call_name, "printfn") == 0)
    {
        for (int i = 0; i < args_size; i++)
        {
            AST_T* visited_ast = visitor_visit(visitor, args[i]);
            switch (visited_ast->type)
            {
                case AST_STRING: printf("%s", visited_ast->string_value); break;
                default: printf("%s", visited_ast->string_value); break;
            }
        }
        return init_ast(AST_NOOP);
    }

    if (strcmp(node->function_call_name, "exit") == 0)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        exit(atoi(visited_ast->string_value));
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
                default: printf("%s", visited_ast->string_value); break;
            }
        }
        scanf("%s", ast->string_value);
        return ast;
    }

    AST_T* fdef = scope_get_function_definition(
        node->scope,
        node->function_call_name
    );

    if (fdef == (void*)0)
    {
        throw_exception("undefined method", node->function_call_name);
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
    switch (node->boolean_operator)
    {
        case BOOLEAN_EQUALTO: node->boolean_value = node->boolean_variable_a == node->boolean_variable_b; break;
        case BOOLEAN_NOTEQUALTO: node->boolean_value = node->boolean_variable_a != node->boolean_variable_b; break;
    }
    printf("%d", node->boolean_value);
    return node;
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
