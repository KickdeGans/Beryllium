#include <stdio.h>
#include <stdlib.h>
#include "main.h"
void throw_exception(char* msg, char* info)
{
    printf("stacktrace in file <%s>: \n  %s '%s'\n", file_path, msg, info);
    exit(1);
}