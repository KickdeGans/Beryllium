#include "builtin.h"
#include "visitor.h"
#include "../core/AST.h"
#include "../lib/io.h"
#include "../lib/string.h"
#include "array.h"
#include "typename.h"
#include "import.h"
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

    if (fast_compare(node->function_call_name, "puts") == 0)
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
            //ast_free(visited_ast);
        }

        //free(args);

        return init_ast(AST_NOOP);
    }
    
    if (fast_compare(node->function_call_name, "exit") == 0)
    {
        if (args_size != 1)
        {
            printf("\n\033[0;31mruntime error:\033[0m\n    function 'exit()' takes 1 argument\n");
            exit(1);
        }

        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        if (visitor->debug_mode)
            printf("\nProcess %i exitted with code %i.\n", getpid(), visited_ast->ast_int);

        int code = visited_ast->ast_int;
        //ast_free(visited_ast);
        //free(args);

        exit(code);
    }

    if (fast_compare(node->function_call_name, "system") == 0)
    {
        if (args_size != 1)
        {
            printf("\n\033[0;31mruntime error:\033[0m\n    function 'system()' takes 1 argument\n");
            exit(1);
        }
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        AST_T* ast = init_ast(AST_INT);
        ast->ast_int = system(visited_ast->string_value);
        //ast_free(visited_ast);
        //free(args);
        return ast;
    }

    if (fast_compare(node->function_call_name, "input") == 0)
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
            
            //ast_free(visited_ast);
        }

        char value[2048];
        fgets(value, sizeof(value), stdin);

        ast->string_value = malloc(2048);
        strcpy(ast->string_value, value);

        ast->string_value[strlen(ast->string_value)-1] = '\0';

        return ast;
    }
    if (fast_compare(node->function_call_name, "sleep") == 0)
    {
        if (args_size != 1)
        {
            printf("\n\033[0;31mruntime error:\033[0m\n    function 'sleep()' takes 1 argument\n");
            exit(1);
        }
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        usleep(visited_ast->ast_double * 1000);
        usleep(visited_ast->ast_int * 1000);

        //free(args);

        return visited_ast;
    }
    if (fast_compare(node->function_call_name, "typeof") == 0)
    {
        if (args_size != 1)
        {
            printf("\n\033[0;31mruntime error:\033[0m\n    function 'typeof()' takes 1 argument\n");
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
            case AST_BOOLEAN: res->ast_int = 0x32003; break;
            default: res->ast_int = -2; break;
        }

        //ast_free(inp);
        free(args);

        return res;
    }
    if (fast_compare(node->function_call_name, "open") == 0)
    {
        if (args_size != 2)
        {
            printf("\n\033[0;31mruntime error:\033[0m\n    function 'open()' takes 2 argument\n");
            exit(1);
        }
        AST_T* ast = init_ast(AST_STREAM);

        ast->stream = fopen(visitor_visit(visitor, args[0])->string_value, visitor_visit(visitor, args[1])->string_value);

        //free(args);

        return ast;
    }
    if (fast_compare(node->function_call_name, "close") == 0)
    {
        if (args_size != 1)
        {
            printf("\n\033[0;31mruntime error:\033[0m\n    function 'close()' takes 1 argument\n");
            exit(1);
        }
        FILE* stream = visitor_visit(visitor, args[0])->stream;

        fclose(stream);

        //free(args);

        return init_ast(AST_NOOP);
    }
    if (fast_compare(node->function_call_name, "write") == 0)
    {
        if (args_size != 2)
        {
            printf("\n\033[0;31mruntime error:\033[0m\n    function 'write()' takes 2 argument\n");
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

            //ast_free(stream);
            //ast_free(value);
            //free(args);

            return ast;
        }
        if (stream->type == AST_ARRAY)
        {
            AST_T* array = stream;

            append_array(array->array_value, value, &stream->array_size);
            
            //ast_free(stream);
            //ast_free(value);
            //free(args);

            return array; 
        }

        fprintf(stream->stream, "%s", value->string_value);

        //ast_free(value);
        //free(args);

        return stream;
    }
    if (fast_compare(node->function_call_name, "read") == 0)
    {
        if (args_size != 1)
        {
            printf("\n\033[0;31mruntime error:\033[0m\n    function 'read()' takes 1 argument\n");
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

            //free(stream);
            //free(args);

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

            //free(stream);
            //free(args);

            return ast;
        }

        return init_ast(AST_NOOP);
    }
    if (fast_compare(node->function_call_name, "remove") == 0)
    {
        char* file = visitor_visit(visitor, args[0])->string_value;
        AST_T* res = init_ast(AST_INT);
        res->ast_int = remove(file);

        //free(args);

        return res;
    }
    if (fast_compare(node->function_call_name, "rand") == 0)
    {
        AST_T* ast = init_ast(AST_INT);

        ast->ast_int = rand();
        
        //free(args);

        return ast;
    }
    if (fast_compare(node->function_call_name, "local_import") == 0)
    {
        char* file = visitor_visit(visitor, args[0])->string_value;
        char path[255] = "/etc/beryllium-lib/";
        strcat(path, replace_char(file, '.', '/'));
        strcat(path, ".ber");
        import(visitor, node->private_scope, path);

        //free(args);

        return init_ast(AST_NOOP);
    }
    if (fast_compare(node->function_call_name, "int") == 0)
    {
        AST_T* ast = visitor_visit(visitor, args[0]);

        AST_T* _int_ = init_ast(AST_INT);
        _int_->ast_int = atoi(ast->string_value);

        //ast_free(ast);
        //free(args);

        return _int_;
    }
    if (fast_compare(node->function_call_name, "string") == 0)
    {
        AST_T* _string_ = init_ast(AST_INT);
        _string_->string_value = malloc(32);
        
        AST_T* visited_ast = visitor_visit(visitor, args[0]);
        switch (visited_ast->type)
        {
            case AST_STRING: sprintf(_string_->string_value,"%s", visited_ast->string_value); break;
            case AST_INT: sprintf(_string_->string_value,"%i", visited_ast->ast_int); break;
            case AST_DOUBLE: sprintf(_string_->string_value, "%f", visited_ast->ast_double); break;
            case AST_BOOLEAN:
                switch (visited_ast->boolean_value)
                {
                    case 1: sprintf(_string_->string_value, "true"); break;
                    case 0: sprintf(_string_->string_value, "false"); break;
                }
                break;
            default: break;
        }

        //ast_free(visited_ast);
        //free(args);

        return _string_;
    }


    return (void*) 0;
}
