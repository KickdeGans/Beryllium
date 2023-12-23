#ifndef EXPORT_H
#define EXPORT_H

#include "AST.h"

void export_compiled_file(char* filename, AST_T* ast);
AST_T* import_compiled_file(char* filename);

#endif