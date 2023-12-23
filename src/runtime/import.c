#include "import.h"
#include "visitor.h"
#include "../compiler/lexer.h"
#include "../compiler/parser.h"
#include "../lib/io.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Import a file */

/* Usage: */
/* import(visitor, scope, "foo.fn"); */
void import(visitor_T* visitor, scope_T* scope, const char* path)
{
    lexer_T* lexer = init_lexer(io_file_read(path));
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser, scope);
    visitor_T* nvisitor = (visitor_T*)calloc(1, sizeof(visitor_T));
    memcpy(nvisitor, visitor, sizeof(&nvisitor));
    nvisitor->scope = scope;
    nvisitor->requires_main_method = 0;

    free(lexer);
    free(parser);
    visitor_visit(nvisitor, root);
    ast_free(root);

    return;
}