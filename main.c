#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "visitor.h"
#include "io.h"
#include "exception.h"
static char* file_path;

int main(int argc, char* argv[])
{
    file_path = argv[1];
    lexer_T* lexer = init_lexer(get_file_contents(argv[1]));
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser, parser->scope);
    visitor_T* visitor = init_visitor();
    visitor_visit(visitor, root);
    throw_exception("test");
    return 0;
}