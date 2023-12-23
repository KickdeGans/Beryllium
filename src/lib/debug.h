#ifndef DEBUG_H
#define DEBUG_H
#include "../core/AST.h"
#include "../runtime/visitor.h"

void AST_analysis(AST_T* ast);
void error_exit(visitor_T* visitor, AST_T* ast);

#endif