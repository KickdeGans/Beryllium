#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "visitor.h"
#include "lib/io.h"

void run(char* filepath)
{
    lexer_T* lexer = init_lexer(io_file_read(filepath));
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser, parser->scope);
    visitor_T* visitor = init_visitor();
    visitor_visit(visitor, root);
}

int main(int argc, char* argv[])
{
    if (strcmp(argv[argc - 1], "--verify") == 0)
    {
        printf("fusion is installed on your system.\n");
        exit(0);
    }
    char* path = argv[argc - 1];
    run(path);
    return 0;
}