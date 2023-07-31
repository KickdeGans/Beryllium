#include "io.h"
#include <stdlib.h>
#include <stdio.h>


char* io_file_read(const char* filepath)
{
    char* buffer = 0;
    size_t length;

    FILE* f = fopen(filepath, "rb");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = calloc(length, length);

        if (buffer)
            fread(buffer, 1, length, f);

        fclose(f);
        return buffer;
    }

    printf("error reading file <%s>\n", filepath);
    exit(2);
}

void io_file_create(const char* filepath)
{
    FILE* f = fopen(filepath, "w");
    fclose(f);
    return;
}

void io_file_delete(const char* filepath)
{
    remove(filepath);
    return;
}