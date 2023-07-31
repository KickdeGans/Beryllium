#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void update()
{
    printf("Update fusion runtime? [y/N] ");

    char choice = getchar();
    if (choice == 'y' || choice == 'Y')
    {
        printf("Getting ready...\n");

        char* clone = "git clone https://www.github.com/KickdeGans/Fusion.git > /dev/null 2>&1";
        char* install = "cd Fusion && sudo make > /dev/null 2>&1";
        char* clean = "rm -rf Fusion > /dev/null 2>&1";
        char* test = "fusion --verify > /dev/null 2>&1";

        printf("Download files...\n");
        system(clone);

        printf("Building...\n");
        system(install);

        printf("Cleaning up...\n");
        system(clean);
        
        printf("Testing...\n");
        if (system(test) == 0)
        {
            printf("Test passed.\n");
        }
        else
        {
            printf("Test failed! Retrying...\n");
            update();
        }

        printf("All done\n");
    }
    else
    {
        printf("Operation canceled by the user\n");
    }
}

int main(int argc, char* argv[])
{
    int option_update = 0;
    int option_help = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--update") == 0)
        {
            option_update = 1;
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            option_help = 1;
        }
    }

    if (option_update)
    {
        update();
    }
    if (option_help)
    {
        printf("Fusion packages help menu:\n    ");
        printf("--update\n        ");
        printf("Update Fusion\n    ");
        printf("--help\n        ");
        printf("Help menu\n");
    }

    return 0;
}