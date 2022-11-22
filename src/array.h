#ifndef ARRAY_H
#define ARRAY_H
#include <stdio.h>
#include "AST.h"

AST_T** init_array();
void append_array(AST_T** array, AST_T* item, size_t* length);

#endif