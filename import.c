#include "import.h"
#include "visitor.h"
#include "lexer.h"
#include "parser.h"
#include "lib/io.h"

void import(const visitor_T* visitor, const char* path)
{
    lexer_T* lexer = init_lexer(io_file_read(path));
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser, visitor->scope);
    visitor_T* new_visitor = init_visitor();
    visitor_visit(new_visitor, root);
    return;
}