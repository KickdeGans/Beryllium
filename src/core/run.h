#ifndef RUN_H
#define RUN_H

#include "AST.h"
#include "../runtime/visitor.h"
#include "../compiler/parser.h"
#include "../compiler/lexer.h"
#include "../lib/io.h"

void set_rand();
void run_file(char* path);
void compile_only(char* path);

#endif