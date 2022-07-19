#include <stdio.h>
#include <stdlib.h>
#include "main.h"
void throw_exception(char* msg)
{
    printf("stacktrace in file <%s>: \n  %s\n", file_path, msg);
    exit(1);
}