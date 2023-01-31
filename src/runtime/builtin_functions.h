#ifndef BUILTIN_FUNCTIONS
#define BUILTIN_FUNCTIONS

#include "../core/AST.h"
#include "visitor.h"

AST_T* try_run_builtin_function(visitor_T* visitor, AST_T* node);

#endif