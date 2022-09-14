#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
void throw_exception(char* msg, char* info)
{
    if (strcmp(info, "") == 0)
    {
        printf("\nerror in file <%s>: \n  %s\n", file_path, msg);
        exit(1);
    }
    printf("\nerror in file <%s>: \n  %s '%s'\n", file_path, msg, info);
    exit(1);
}