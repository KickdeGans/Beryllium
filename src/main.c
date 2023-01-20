#include "AST.h"
#include "lexer.h"
#include "parser.h"
#include "scope.h"
#include "visitor.h"
#include "lib/io.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

void set_rand()
{
    struct timeval time;

    gettimeofday(&time, NULL);

    size_t s1 = (size_t)(time.tv_sec) * 1000;
    size_t s2 = (time.tv_usec / 1000);

    srand(s1 + s2);

    return;
}

void print_help()
{
    printf("Usage: fusion <options> <file>\n\n--add-library:\n    Add a code file to the library.\n\n--verify:\n    Verify if fusion is installed.\n\n--help:\n    Open help menu.\n");
    return;
}

/* Load file to run */
void run(char* filepath)
{
    char* file_contents = io_file_read(filepath);

    lexer_T* lexer = init_lexer(file_contents);
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser, parser->scope);
    visitor_T* visitor = init_visitor();

    free(lexer);
    free(parser);
    free(file_contents);
    set_rand();

    visitor_visit(visitor, root);

    ast_free(root);
    free(visitor);

    return;
}

/* Main function */
int main(int argc, char* argv[])
{
    if (argc == 1)
    {
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
        else
        {
            char* path = argv[i];
            run(path);  
        }
    }
    
    return 0;
}