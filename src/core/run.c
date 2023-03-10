#include "run.h"
#include "AST.h"
#include "../runtime/visitor.h"
#include "../compiler/parser.h"
#include "../compiler/lexer.h"
#include "../lib/io.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

void set_rand()
{
    struct timeval time;

    gettimeofday(&time, NULL);

    size_t s1 = (size_t)(time.tv_sec) * 1000;
    size_t s2 = (time.tv_usec / 1000);

    srand(s1 + s2);

    return;
}

void run_file(char* path)
{
    char* file_contents = io_file_read(path);

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

void compile_only(char* path)
{
    char* file_contents = io_file_read(path);

    lexer_T* lexer = init_lexer(file_contents);
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser, parser->scope);

    free(lexer);
    free(parser);
    free(file_contents);
    ast_free(root);

    return;
}