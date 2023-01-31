#include "builtin_functions.h"
#include "visitor.h"
#include "../core/AST.h"
#include "../lib/io.h"
#include "thread.h"
#include "../lib/http.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

AST_T* try_run_builtin_function(visitor_T* visitor, AST_T* node)
{
    AST_T** args = node->function_call_arguments;
    size_t args_size = node->function_call_arguments_size;
    
    if (strcmp(node->function_call_name, "print") == 0)
    {
        for (size_t i = 0; i < args_size; i++)
        {
            AST_T* visited_ast = visitor_visit(visitor, args[i]);
            switch (visited_ast->type)
            {
                case AST_STRING: printf("%s", visited_ast->string_value); break;
                case AST_INT: printf("%i", visited_ast->ast_int); break;
                case AST_DOUBLE: printf("%f", visited_ast->ast_double); break;
                case AST_BOOLEAN:
                    switch (visited_ast->boolean_value)
                    {
                        case 1: printf("true"); break;
                        case 0: printf("false"); break;
                    }
                    break;
                default: break;
            }
        }
        return init_ast(AST_NOOP);
    }
    if (strcmp(node->function_call_name, "println") == 0)
    {
        for (size_t i = 0; i < args_size; i++)
        {
            AST_T* visited_ast = visitor_visit(visitor, args[i]);
            switch (visited_ast->type)
            {
                case AST_STRING: printf("%s", visited_ast->string_value); break;
                case AST_INT: printf("%i", visited_ast->ast_int); break;
                case AST_DOUBLE: printf("%f", visited_ast->ast_double); break;
                case AST_BOOLEAN:
                    switch (visited_ast->boolean_value)
                    {
                        case 1: printf("true"); break;
                        case 0: printf("false"); break;
                    }
                    break;
                default: break;
            }
        }
        printf("\n");
        return init_ast(AST_NOOP);
    }

    if (strcmp(node->function_call_name, "exit") == 0)
    {
        if (args_size != 1)
        {
            printf("\nruntime error:\n    function 'exit()' takes 1 argument\n");
            exit(1);
        }

        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        exit(visited_ast->ast_int);
    }

    if (strcmp(node->function_call_name, "system") == 0)
    {
        if (args_size != 1)
        {
            printf("\nruntime error:\n    function 'system()' takes 1 argument\n");
            exit(1);
        }
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = system(visited_ast->string_value);
        return ast;
    }

    if (strcmp(node->function_call_name, "input") == 0)
    {
        AST_T* ast = init_ast(AST_STRING);
        for (size_t i = 0; i < args_size; i++)
        {
            AST_T* visited_ast = visitor_visit(visitor, args[i]);
            switch (visited_ast->type)
            {
                case AST_STRING: printf("%s", visited_ast->string_value); break;
                case AST_INT: printf("%i", visited_ast->ast_int); break;
                case AST_DOUBLE: printf("%f", visited_ast->ast_double); break;
                case AST_BOOLEAN:
                    switch (visited_ast->boolean_value)
                    {
                        case 1: printf("true"); break;
                        case 0: printf("false"); break;
                    }
                    break;
                default: break;
            }
        }
        char value[16384];
        fgets(value, sizeof(value), stdin);
        ast->string_value = malloc(sizeof(value));
        strcpy(ast->string_value, value);
        ast->string_value[strlen(ast->string_value)] = '\0';
        ast->string_value[strlen(ast->string_value) - 1] = '\0';
        return ast;
    }
    if (strcmp(node->function_call_name, "sleep") == 0)
    {
        if (args_size != 1)
        {
            printf("\nruntime error:\n    function 'sleep()' takes 1 argument\n");
            exit(1);
        }
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        usleep(visited_ast->ast_double * 1000);
        usleep(visited_ast->ast_int * 1000);
        return visited_ast;
    }
    if (strcmp(node->function_call_name, "http") == 0)
    {
        AST_T* header = visitor_visit(visitor, args[0]);
        AST_T* ast = init_ast(AST_STRING);
        ast->string_value = http_request(header->string_value);
        return ast;
    }
    if (strcmp(node->function_call_name, "strcat") == 0)
    {
        if (args_size != 2)
        {
            printf("\nruntime error:\n    function 'strcat()' takes 2 arguments\n");
            exit(1);
        }
        AST_T* a = visitor_visit(visitor, args[0]);
        AST_T* b = visitor_visit(visitor, args[1]);
        char* res = malloc(sizeof(a->string_value)+sizeof(b->string_value)+1);
        sprintf(res, "%s%s", a->string_value, b->string_value);
        AST_T* ast = init_ast(AST_STRING);
        ast->string_value = res;
        return ast;
    }
    if (strcmp(node->function_call_name, "typeof") == 0)
    {
        if (args_size != 1)
        {
            printf("\nruntime error:\n    function 'typeof()' takes 1 argument\n");
            exit(1);
        }
        AST_T* inp = visitor_visit(visitor, args[0]);
        AST_T* res = init_ast(AST_INT);
        switch (inp->type)
        {
            case AST_STRING: res->ast_int = 0x32002; break;
            case AST_INT: res->ast_int = 0x32000; break;
            case AST_DOUBLE: res->ast_int = 0x32001; break;
            case AST_LONG: res->ast_int = 0x32006; break;
            case AST_ARRAY: res->ast_int = 0x32004; break;
            case AST_STREAM: res->ast_int = 0x32005; break;
            default: res->ast_int = -2; break;
        }
        return res;
    }
    if (strcmp(node->function_call_name, "open") == 0)
    {
        if (args_size != 2)
        {
            printf("\nruntime error:\n    function 'open()' takes 2 argument\n");
            exit(1);
        }
        AST_T* ast = init_ast(AST_STREAM);

        ast->stream = fopen(visitor_visit(visitor, args[0])->string_value, visitor_visit(visitor, args[1])->string_value);

        return ast;
    }
    if (strcmp(node->function_call_name, "close") == 0)
    {
        if (args_size != 1)
        {
            printf("\nruntime error:\n    function 'close()' takes 1 argument\n");
            exit(1);
        }
        FILE* stream = visitor_visit(visitor, args[0])->stream;

        fclose(stream);

        return init_ast(AST_NOOP);
    }
    if (strcmp(node->function_call_name, "write") == 0)
    {
        if (args_size != 2)
        {
            printf("\nruntime error:\n    function 'write()' takes 2 argument\n");
            exit(1);
        }
        AST_T* stream = visitor_visit(visitor, args[0]);
        AST_T* value = visitor_visit(visitor, args[1]);
        
        if (stream->type == AST_STRING)
        {
            char* string = calloc(sizeof(stream->string_value) + sizeof(value->string_value) + 1, sizeof(char*));

            sprintf(string, "%s%s", stream->string_value, value->string_value);

            AST_T* ast = init_ast(AST_STRING);
            ast->string_value = string;

            return ast;
        }

        fprintf(stream->stream, value->string_value);

        return init_ast(AST_NOOP);
    }
    if (strcmp(node->function_call_name, "read") == 0)
    {
        if (args_size != 1)
        {
            printf("\nruntime error:\n    function 'read()' takes 1 argument\n");
            exit(1);
        }
        FILE* stream = visitor_visit(visitor, args[0])->stream;
        
        if (stream == stdin)
        {
            AST_T* ast = init_ast(AST_STRING);

            char value[16384];

            fgets(value, sizeof(value), stdin);

            ast->string_value = malloc(sizeof(value));

            strcpy(ast->string_value, value);

            ast->string_value[strlen(ast->string_value)] = '\0';
            ast->string_value[strlen(ast->string_value) - 1] = '\0';

            return ast;
        }

        char* buffer = 0;
        size_t length;

        if (stream)
        {
            fseek(stream, 0, SEEK_END);
            length = ftell(stream);
            fseek(stream, 0, SEEK_SET);

            buffer = calloc(length, length);

            if (buffer)
                fread(buffer, 1, length, stream);

            AST_T* ast = init_ast(AST_STRING);
            ast->string_value = buffer;

            return ast;
        }

        return init_ast(AST_NOOP);
    }
    if (strcmp(node->function_call_name, "remove") == 0)
    {
        char* file = visitor_visit(visitor, args[0])->string_value;
        AST_T* res = init_ast(AST_INT);
        res->ast_int = remove(file);
        return res;
    }
    if (strcmp(node->function_call_name, "rand") == 0)
    {
        AST_T* ast = init_ast(AST_INT);

        ast->ast_int = rand();
        
        return ast;
    }
    
    return (void*) 0;
}
