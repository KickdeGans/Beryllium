#include "import.h"
#include "visitor.h"
#include "lexer.h"
#include "parser.h"
#include "lib/io.h"

void import(visitor_T* visitor, const char* path)
{
    lexer_T* lexer = init_lexer(io_file_read(path));
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser, visitor->scope);
    visitor_visit(visitor, root);
    return;
}