#include <stdlib.h>
#include "memory.h"
#include "../core/AST.h"

int ast_sanity_check(AST_T* ast)
{
    if (ast == (void*) 0)
        return 0;

    if (ast->type > AST_NOOP)
        return 0;

    if (ast->type < 0)
        return 0;

    return 1;
}