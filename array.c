#include <stdio.h>
#include "AST.h"


AST_T** init_array()
{
    AST_T** array;
    array = calloc(1, sizeof(struct AST_STRUCT**));
    return array;
}
void append_array(AST_T** array, AST_T* item, size_t* length)
{
    *length += 1;
    array = realloc(
        array,
        *length * sizeof(struct AST_STRUCT**)
    );
    array[*length - 1] = item;
    printf("appended array, new length is %i\n", length);
}