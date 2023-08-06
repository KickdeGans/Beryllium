#include "main.h"
#include "core/run.h"
#include "core/AST.h"
#include "core/scope.h"
#include "runtime/visitor.h"
#include "compiler/parser.h"
#include "compiler/lexer.h"
#include "lib/string.h"
#include "lib/io.h"
#include "core/export.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define VERSION "0.4"

void print_help()
{
    printf("Usage: fusion <options> <file>\n--version:\n    Print version.\n--help:\n    Open help menu.\n");
    return;
}

/* Main function */
int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("No arguments or file given.\nType 'fusion --help' to open the help menu.\n");
        exit(0);
    }
    
    int   __version       = 0;
    int   __compile       = 0;
    int   __run           = 0;
    int   __help          = 0;
    char* compile_file    = 0;

    for (int i = 1; i < argc; i++)
    {
        FCARG("--version",__version) __version = 1; 
        FCARG("--compile",__compile) FCARG_ACTION(__compile);
        FCARG("--run",__run)  FCARG_ACTION(__run);
        FCARG("--help",__help) __help = 1;
    }
    if (__version)
    {
        #if defined(__clang__)
            #if defined(__gnu_linux__)
                printf("Fusion runtime %s [clang] on Linux\n", VERSION);
                goto endver;
            #endif
            #if defined(__unix)
                printf("Fusion runtime %s [clang] on Unix\n", VERSION);
                goto endver;
            #endif
            #if defined(_WIN32)
                printf("Fusion runtime %s [clang] on Windows\n", VERSION);
                goto endver;
            #endif
        #elif defined(__GNUC__) || defined(__GNUG__)
            #if defined(__linux)
                printf("Fusion runtime %s [gcc] on Linux\n", VERSION);
                goto endver;
            #endif
            #if defined(__unix)
                printf("Fusion runtime %s [gcc] on Unix\n", VERSION);
                goto endver;
            #endif
            #if defined(_WIN32)
                printf("Fusion runtime %s [gcc] on Windows\n", VERSION);
                goto endver;
            #endif
        #elif defined(_MSC_VER)
        printf("Fusion runtime %s [Microsoft C] on Windows\n", VERSION);
        #endif
    }
    endver:
    if (__compile)
    {
        char* file_contents = io_file_read(compile_file);

        lexer_T* lexer = init_lexer(file_contents);
        parser_T* parser = init_parser(lexer);
        AST_T* root = parser_parse(parser, parser->scope);

        free(lexer);
        free(parser);
        free(file_contents);

        export_compiled_file("a.out", root);
    }
    if (__run)
    {
        char* file_contents = io_file_read(compile_file);

        AST_T* root = import_compiled_file(compile_file);

        visitor_T* visitor = init_visitor();

        free(file_contents);
        set_rand();

        visitor_visit(visitor, root);

        ast_free(root);
        free(visitor);
    }
    if (__help)
    {
        print_help();
    }
    if (__version || __compile || __run || __help)
    {
        exit(0);
    }

    char* path = calloc(1, sizeof(char*));

    for (int i = 1; i < argc; i++)
    {
        int len = strlen(argv[i]);
        char* last_four = &argv[i][len-3];

        path = realloc(path, sizeof(argv[i]) + 1);

        strcat(path, argv[i]);
        strcat(path, " ");

        if (fast_compare(last_four,".fn") == 0 || i + 1 == argc)
        {
            path[strlen(path)-1] = '\0';

            if (i == argc)
                run_file(path, argv);
            else
                run_file(path, calloc(1, sizeof(char*)));
        }
    }

    return 0;
}
