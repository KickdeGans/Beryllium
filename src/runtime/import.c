#include "import.h"
#include "visitor.h"
#include "../compiler/lexer.h"
#include "../compiler/parser.h"
#include "../lib/io.h"

/* Import a file */
/* Usage: */
/* import(visitor, "foo.fn"); */
void import(visitor_T* visitor, const char* path)
{
    lexer_T* lexer = init_lexer(io_file_read(path));
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser, visitor->scope);
    
    free(lexer);
    free(parser);
    visitor_visit(visitor, root);
    ast_free(root);

    return;
}