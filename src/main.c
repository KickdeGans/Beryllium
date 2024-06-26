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
#include <unistd.h>

#define VERSION "0.5"

void print_help(void)
{
    printf("Usage: beryllium <options> <file>\nInterpreter for the Beryllium programming language.\n\n--version:\n    Print version.\n--help:\n    Open help menu.\n--mkexec:\n    Make executable like file.\n    Usage: beryllium --mkexec [FILE]\n   The resulting file is called 'a.out', make sure you have backed up any files called 'a.out'.\n--debug-mode:\n    Run in debug mode.\n   This will result in a dump of the faulty AST (abstract syntax tree), which contains useful information for debugging.\n");
    printf("\nGitHub page: https://github.com/KickdeGans/Beryllium\n");
    printf("Documentation: https://github.com/KickdeGans/Beryllium/wiki\n");
    printf("Submit issues at: https://github.com/KickdeGans/Beryllium/issues\n");
    return;
}

void print_version(void)
{
        #if defined(__clang__)
            #if defined(__gnu_linux__)
                printf("beryllium runtime %s [clang] on Linux\n", VERSION);
                return;
            #endif
            #if defined(__unix)
                printf("beryllium runtime %s [clang] on Unix\n", VERSION);
                return;
            #endif
            #if defined(_WIN32)
                printf("beryllium runtime %s [clang] on Windows\n", VERSION);
                return;
            #endif
        #elif defined(__GNUC__) || defined(__GNUG__)
            #if defined(__linux)
                printf("beryllium runtime %s [gcc] on Linux\n", VERSION);
                return;
            #endif
            #if defined(__unix)
                printf("beryllium runtime %s [gcc] on Unix\n", VERSION);
                return;
            #endif
            #if defined(_WIN32)
                printf("beryllium runtime %s [gcc] on Windows\n", VERSION);
                return;
            #endif
        #elif defined(_MSC_VER)
        printf("beryllium runtime %s [Microsoft C] on Windows\n", VERSION);
        #endif
}

/* Main function */
int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        print_version();
        printf("No arguments or file given.\n\nType 'beryllium --help' to open the help menu.\n");
        exit(1);
    }
    
    int   __version         = 0;
    int   __compile         = 0;
    int   __run             = 0;
    int   __help            = 0;
    int   __debug_mode      = 0;
    int   __make_program    = 0;
    int   file_location_pos = 1;
    char* compile_file      = 0;

    for (int i = 1; i < argc; i++)
    {
        FCARG("--version",__version) __version = 1; 
        FCARG("--compile",__compile) FCARG_ACTION(__compile);
        FCARG("--run",__run)  FCARG_ACTION(__run);
        FCARG("--help",__help) __help = 1;
        FCARG("--debug-mode",__debug_mode) __debug_mode = 1;
        FCARG("--mkexec",__debug_mode) FCARG_ACTION(__make_program);
    }
    if (__version)
    {
        print_version();
    }
    if (__make_program)
    {
        char* file_contents = io_file_read(compile_file);

        FILE* f = fopen("main", "w");

        fprintf(f, "#!/usr/bin/beryllium\n\n%s", file_contents);

        int close = fclose(f);
        close += system("chmod +x ./main");

        return close;
    }
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

    if (__debug_mode)
        file_location_pos = 2;

    for (int i = file_location_pos; i < argc; i++)
    {
        int len = strlen(argv[i]);
        char* last_four = &argv[i][len-3];

        path = realloc(path, sizeof(argv[i]) + 1);

        strcat(path, argv[i]);
        strcat(path, " ");

        if (fast_compare(last_four,".ber") == 0 || i + 1 == argc)
        {
            if (__debug_mode)
                printf("###### RUNNING IN DEBUG MODE ######\n\n");

            path[strlen(path)-1] = '\0';

            if (i == argc)
                run_file(path, argv, __debug_mode);
            else
                run_file(path, calloc(1, sizeof(char*)), __debug_mode);

            if (__debug_mode)
                printf("\nProcess %i exited with code 0.\n", getpid());
        }
    }

    return 0;
}
