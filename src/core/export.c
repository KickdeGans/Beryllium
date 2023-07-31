#include <stdio.h>
#include <stdlib.h>
#include "export.h"

void export_compiled_file(char* filename, AST_T* ast)
{
    FILE* output;

    output = fopen(filename, "wb");
    if (output == NULL) 
    {
        printf("Error opening file %s\n", filename);
        exit(1);
    }
 
    int flag = 0;
    flag = fwrite(ast, sizeof(AST_T), 1,
                  output);
    if (!flag)
    {
        printf("Error creating binary file.\n");
        exit(1);
    }

    fclose(output);
}

AST_T* import_compiled_file(char* filename)
{
    FILE* input;
 
    // Open person.dat for reading
    input = fopen(filename, "rb");
    if (input == NULL) 
    {
        printf("Error opening file %s\n", filename);
        exit(1);
    }
 
    AST_T* read_struct;

    rewind(input);

    fread(&read_struct, sizeof(AST_T), 1, input);

    fclose(input);

    printf("%i\n", read_struct->type);

    if (!read_struct)
    {
        printf("Cannot read binary file. It may be corrupted or empty\n");
        exit(1);
    }

    return read_struct;
}