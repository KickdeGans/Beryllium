#include "main.h"
#include "core/run.h"
#include "core/AST.h"
#include "runtime/visitor.h"
#include "compiler/parser.h"
#include "compiler/lexer.h"
#include "lib/io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

void print_help()
{
    printf("Usage: fusion <options> <file>\n\n--verify:\n    Verify if fusion is installed.\n\n--help:\n    Open help menu.\n");
    return;
}

/* Main function */
int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("No options\n");
        print_help();
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--verify") == 0)
        {
            printf("Fusion is installed on your system.\n'Hello world!' should be printed if its properly installed.\n");
            printf("Process exitted with code %i.\n", system("fusion ~/hello.fn"));
            exit(0);
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            print_help();
        }
    }

    run_file(argv[argc-1]);

    return 0;
}