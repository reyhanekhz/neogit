#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define MAX_LENGTH 1000


int addAlias(char* alias, char* command){
    FILE* file = fopen (".neogit/aliases", "a");
    if (file == NULL){
        perror ("Error opening aliases file");
        return 1;
    }

    fprintf(file, "%s : %s\n", alias, command);
    fclose(file);
    return 0;

}


int execAlias(char* alias){
    char line[MAX_LENGTH];
    char orgAlias[MAX_LENGTH];
    char command[MAX_LENGTH];

    FILE* file = fopen(".neogit/aliases", "r");

    while ( fgets(line, MAX_LENGTH, file) != NULL){
        sscanf(line, "%s : %[^\n]s\n", orgAlias, command);
        if (strcmp(orgAlias, alias) == 0){
            system(command);
            return 0;
        }
    }

    perror ("Invalid alias");
    return 1;

}


int globalConfig(const char *key, const char *value) {
    if (strcmp(key, "user.name") == 0)
    {
        FILE* file = fopen(".neogit/config/username", "w");
        if (file == NULL) {
            perror("error opening username config file");
            return 1;
        }

        fprintf(file, "username: %s\n", value);
        fclose(file);
        return 0;
    }
    else{
        FILE* file = fopen(".neogit/config/useremail", "w");
        if (file == NULL) {
            perror("error opening useremail config file");
            return 1;
        }

        fprintf(file, "useremail: %s\n", value);
        fclose(file);
        return 0;
    }

}


int runInit() {
    char parentdir[MAX_LENGTH];
    char currentdir[MAX_LENGTH];
    if (getcwd(currentdir, MAX_LENGTH) == NULL)
        return 1;


    do{
        if (access(".neogit", F_OK) == 0){
        perror("Neogit repository already exists");
        return 1;
        }

        if (chdir("..") != 0)
            return 1;

        if (getcwd(parentdir, MAX_LENGTH) == NULL)
            return 1;


    } while (strcmp(parentdir, "/") != 0);

    if (chdir(currentdir) != 0)
        return 1;

    if (mkdir (".neogit", 0755) != 0){
        perror("Error creating Neogit directory");
        return 1;
    }


    if(mkdir(".neogit/config", 0755) != 0){
        perror("Error creating config directory");
        return 1;
    }

    FILE* file = fopen (".neogit/config/username", "w");
    fclose(file);

    file = fopen (".neogit/config/useremail", "w");
    fclose(file);

    file = fopen (".neogit/staging", "w");
    fclose(file);

    file = fopen(".neogit/tracks", "w");
    fclose(file);


    printf("Empty Neogit repository initialized in %s\n", currentdir);
    return 0;

}


int addToStaging(char* filepath){
    FILE* file = fopen (".neogit/staging", "r");
    if (file == NULL)
        return 1;

    char line[MAX_LENGTH];
    char path[MAX_LENGTH];
    sprintf(path, "%s\n", filepath);

    while (fgets(line, MAX_LENGTH, file) != NULL){
        if (strcmp(line, path) == 0){
            printf("File already added to staging area");
            return 0;
        }
    }
    fclose (file);

    file = fopen (".neogit/staging", "w");
    if (file == NULL)
        return 1;

    fprintf(file, "%s\n", path);
    fclose (file);
    return 0;

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
        return addAlias(argv[2], argv[3]);
    else if (strcmp(argv[1], "init") == 0)
        return runInit();
    else if (strcmp(argv[1], "add") == 0){
        if (argc == 2){
            perror ("No files specified");
            return 1;
        }
        return addToStaging(argv[2]);
    }



    else
        return execAlias(argv[1]);

    return 0;

}
