#include <stdio.h>
#include <string.h>
#include "AST.h"
#include "lexer.h"
#include "parser.h"
#include "scope.h"
#include "visitor.h"
#include "lib/io.h"

/* Load file to run */
void run(char* filepath)
{
    char* file_contents = io_file_read(filepath);

    lexer_T* lexer = init_lexer(file_contents);
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser, parser->scope);
    visitor_T* visitor = init_visitor();

    visitor_visit(visitor, root);

    free(lexer);
    free(parser);
    ast_free(root);
    free(visitor);

    return;
}

/* Main function */
int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--verify") == 0)
        {
            printf("fusion is installed on your system.\n");
            exit(0);
        }
        else if (strcmp(argv[i], "--add-library") == 0)
        {
            #ifdef linux
                i += 1;
                char command[1024] = "cp ";

                strcat(command, argv[i]);
                strcat(command, " /bin/fusion-lib/");
                system(command);

                continue;
            #endif
            #ifdef _WIN32
                i += 1;
                char command[1024] = "copy ";

                strcat(command, argv[i]);
                strcat(command, " C:\\Windows\\fusion-lib\\");
                system(command);

                continue;
            #endif
        }
        else
        {
            char* path = argv[i];
            run(path);  
        }
    }
    
    return 0;
}