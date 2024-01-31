#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int config(const char *key, const char *value)
{
    if ( mkdir (".neogit/.config_folder", 0755) != 0)
    {
        perror("error creating config directory");
        return 1;
    }

    FILE* file = fopen (".neogit/.config_folder/config", "a");
    if (file == NULL)
    {
        perror ("error creating config file");
        return 1;
    }

    fprintf( file, "%s %s\n", key, value);
    fclose (file);
    return 0;

}



//
//
//int falias(char alias_code[MAX_COMMAND_LENGTH], char qcommand[MAX_COMMAND_LENGTH])
//{
//    char alias[MAX_COMMAND_LENGTH];
//    char command[MAX_COMMAND_LENGTH];
//    sscanf(alias_code, "alias.%s", alias);
//    sscanf(qcommand, "\"%s\"", command);
//    FILE* file = fopen("alias", "w");
//    fprintf(file, "%s = %s\n", alias, command);
//    return 0;
//}
//
//int aliascommand(char alias[MAX_COMMAND_LENGTH])
//{
//    FILE *fp;
//    char aliases[MAX_LINE_LENGTH][MAX_LINE_LENGTH];
//    char command[MAX_LINE_LENGTH];
//    char *token;
//    int i = 0;
//
//    fp = fopen("aliases", "r");
//    if (fp == NULL) {
//        perror("Error opening aliases file");
//        return 1;
//    }
//
//    while (fgets(aliases[i], MAX_LINE_LENGTH, fp) != NULL) {
//        aliases[i][strcspn(aliases[i], "\n")] = '\0';
//        i++;
//    }
//
//    fclose(fp);
//
//    while (1) {
//        printf("> ");
//        if (fgets(command, MAX_LINE_LENGTH, stdin) == NULL)
//            break;
//
//        command[strcspn(command, "\n")] = '\0';
//        for (int j = 0; j < i; j++)
//        {
//            token = strtok(aliases[j], "=");
//            if (strcmp(token, command) == 0) {
//                system(strtok(NULL, "=\n"));
//                break;
//            }
//        }
//        return 0;
//    }
//}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        perror("invalid command");
        return 1;
    }

    if (strcmp(argv[1], "config") == 0)
    {
        return config(argv[3], argv[4]);
    }


    return 0;

}
