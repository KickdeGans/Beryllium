#include <stdio.h>
#include "main.h"
void throw_exception(char* msg)
{
    printf("stacktrace in file <%s>: \n  %s\n", file_path, msg);
    return;
}