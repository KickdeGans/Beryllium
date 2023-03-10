#include "main.h"
#include "core/run.h"
#include "core/AST.h"
#include "core/scope.h"
#include "runtime/visitor.h"
#include "compiler/parser.h"
#include "compiler/lexer.h"
#include "lib/io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define VERSION "0.3.3"
#define FUSION_NAME "Fusion runtime [GCC]"

void print_help()
{
    printf("Usage: fusion <options> <file>\n--verify:\n    Verify if fusion is installed.\n--version:\n    Print version.\n--compile-only:\n    Only compile file.\n--help:\n    Open help menu.\n");
    return;
}

void bundle(char* path)
{
    char* cmd = malloc(2048);

    if (strcmp(path, "") == 0)
        sprintf(cmd, "echo \"#!/bin/fusion\" > out\necho \"bundle();\" >> out\ncat *.fn >> ../out\ncd ..\nchmod +x out");
    else
        sprintf(cmd, "echo \"#!/bin/fusion\" > out\necho \"bundle();\" >> out\ncd %s\ncat *.fn >> ../out\ncd ..\necho \"main();\" >> out\nchmod +x out", path);

    printf("%s\n", cmd);

    system(cmd);
}

/* Main function */
int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("No arguments or file given.\nUse argument --help to open help menu\n");
        exit(0);
    }

    int   __verify        = 0;
    int   __version       = 0;
    int   __bundle        = 0;
    int   __help          = 0;
    char* __bundle_path   = "";

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--verify") == 0)
        {
            __verify = 1;
        }
        if (strcmp(argv[1], "--version") == 0)
        {
            __version = 1;
        }
        if (strcmp(argv[1], "--bundle") == 0)
        {
            __bundle = 1;

            if (i != argc)
            {
                i++;
                __bundle_path = argv[i];
            }
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            __help = 1;
        }
    }

    if (__verify)
    {
        printf("Fusion is installed on your system.\n'Hello world!' should be printed if its properly installed.\n");
        printf("Process exitted with code %i.\n", system("fusion ~/.hello.fn"));
    }
    if (__version)
    {
        printf("%s version %s\n", FUSION_NAME, VERSION);
    }
    if (__bundle)
    {
        bundle(__bundle_path);
    }
    if (__help)
    {
        print_help();
    }
    if (__verify || __version || __help || __bundle)
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

        if (strcmp(last_four,".fn") == 0 || i + 1 == argc)
        {
            path[strlen(path)-1] = '\0';

            run_file(path);
        }
    }

    return 0;
}