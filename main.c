#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define MAX_ALIASES 128

struct alias_cmd {
    char *alias;
    char *command;
};

struct alias_cmd aliases[MAX_ALIASES];
int num_aliases = 0;

int num_config = 0;

void add_alias(char *alias, char *command) {
    strcpy(aliases[num_aliases].alias, alias);
    strcpy(aliases[num_aliases].command, command);
    num_aliases++;
}


void exec_alias(char *alias) {
    for (int i = 0; i < num_aliases; i++) {
        if (strcmp(aliases[i].alias, alias) == 0) {
            char cmd[1024];
            strcpy(cmd, aliases[i].command);
            system(cmd);
            break;
        }
    }
}

int globalConfig(const char *key, const char *value) {

    if (access(".neogit/config", F_OK) == -1) {
        if (mkdir(".neogit/config", 0755) != 0) {
            perror("error creating config directory");
            return 1;
        }
    }

    if (strcmp(key, "user.name") == 0)
    {
        FILE* file = fopen(".neogit/config/username", "w");
        if (file == NULL) {
            perror("error opening username config file");
            return 1;
        }

        fprintf(file, "username: %s", value);
        fclose(file);
        return 0;
    }
    else{
        FILE* file = fopen(".neogit/config/useremail", "w");
        if (file == NULL) {
            perror("error opening useremail config file");
            return 1;
        }

        fprintf(file, "useremail: %s", value);
        fclose(file);
        return 0;
    }

}



int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        perror("invalid command");
        return 1;
    }

    if (strcmp(argv[1], "config") == 0)
        return globalConfig(argv[3], argv[4]);

    else if (strcmp(argv[1], "alias") == 0)
        add_alias(argv[2], argv[3]);

    else
        exec_alias(argv[1]);




    return 0;

}
