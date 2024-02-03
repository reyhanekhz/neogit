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
    if (access(".neogit", F_OK) != 0){
        perror("No initialized repository");
        return 1;
    }

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
    if (access(".neogit", F_OK) != 0){
        perror("No initialized repository");
        return 1;
    }

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
    if (access(".neogit", F_OK) != 0){
        perror("No initialized repository");
        return 1;
    }

    if (strcmp(key, "user.name") == 0)
    {
        FILE* file = fopen("/home/reyhane/.neogitGlobalSettings/globalConfig/globalUsername", "w");
        if (file == NULL) {
            perror("error opening username global config file");
            return 1;
        }

        fprintf(file, "username: %s\n", value);
        fclose(file);
        return 0;
    }
    else{
        FILE* file = fopen("/home/reyhane/.neogitGlobalSettings/globalConfig/globalUseremail", "w");
        if (file == NULL) {
            perror("error opening useremail global config file");
            return 1;
        }

        fprintf(file, "useremail: %s\n", value);
        fclose(file);
        return 0;
    }

}

int localConfig(char* key, char* value){
    if (access(".neogit", F_OK) != 0){
        perror("No initialized repository");
        return 1;
    }

    if (strcmp(key, "user.name") == 0)
    {
        FILE* file = fopen (".neogit/localConfig/localUsername", "w");
        if (file == NULL){
            perror("Error opening local username file");
            return 1;
        }
        fprintf(file, "local username : %s\n", value);
        fclose (file);
    }
    if (strcmp(key, "user.email") == 0)
    {
        FILE* file = fopen (".neogit/localConfig/localUseremail", "w");
        if (file == NULL){
            perror("Error opening local useremail file");
            return 1;
        }
        fprintf(file, "local useremail : %s\n", value);
        fclose (file);
    }
    return 0;
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

    if (access("/home/reyhane/.neogitGlobalSettings", F_OK) != 0){
        if (mkdir("/home/reyhane/.neogitGlobalSettings", 0755) != 0){
            perror ("Error creating global settings directory");
            return 1;
        }
        else {
            if (mkdir ("/home/reyhane/.neogitGlobalSettings/globalConfig", 0755) ==0){
                FILE* file = fopen("/home/reyhane/.neogitGlobalSettings/globalConfig/globalUsername", "w");
                fclose (file);
                file = fopen("/home/reyhane/.neogitGlobalSettings/globalConfig/globalUseremail", "w");
                fclose(file);
            }
            else{
                perror ("Error creating global config directory");
                return 1;
            }
        }
    }

    if(mkdir(".neogit/localConfig", 0755) != 0){
        perror("Error creating commit directory");
        return 1;
    }

    if(mkdir(".neogit/commits", 0755) != 0){
        perror("Error creating commit directory");
        return 1;
    }
    if(mkdir(".neogit/files", 0755) != 0){
        perror("Error creating files directory");
        return 1;
    }

    FILE* file = fopen (".neogit/trackedFiles", "w");
    fclose(file);
    file = fopen (".neogit/staging", "w");
    fclose(file);
    file = fopen (".neogit/localConfig/localUsername", "w");
    fclose(file);
    file = fopen (".neogit/localConfig/localUseremail", "w");
    fclose(file);
    file = fopen(".neogit/tracks", "w");
    fclose(file);
    file = fopen(".neogit/commitID", "w");
    fclose(file);


    printf("Empty Neogit repository initialized in %s\n", currentdir);
    return 0;

}

void addDirToStaging(char* directory){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    FILE* file = fopen(".neogit/staging", "r");
    DIR *dir = opendir(directory);
    struct dirent* entry;
    if (dir == NULL) {
        printf("Error opening current directory\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG){
            strcat(entry->d_name, "\n");
            char line[MAX_LENGTH];

            while (fgets(line, MAX_LENGTH, file) != NULL){
                if (strcmp(line, entry->d_name) == 0){
                printf("File already added to staging area\n");
                return;
                }
            }
            fclose(file);

            file = fopen (".neogit/staging", "a");
            fprintf(file, "%s", entry->d_name);
            fclose(file);
        }
    }

    closedir(dir);

}

void addFileToStaging(char* filepath){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    FILE* file = fopen (".neogit/staging", "r");
    if (file == NULL)
        return;

    char line[MAX_LENGTH];
    strcat(filepath, "\n");

    while (fgets(line, MAX_LENGTH, file) != NULL){
        if (strcmp(line, filepath) == 0){
            printf("File already added to staging area\n");
            return;
        }
    }
    fclose (file);

    file = fopen (".neogit/staging", "a");
    if (file == NULL)
        return;

    fprintf(file, "%s", filepath);
    fclose (file);

}

int commit(){
    if (access(".neogit", F_OK) != 0){
        perror("No initialized repository");
        return 1;
    }

    FILE* file = fopen(".neogit/staging", "r");
    char filepath[MAX_LENGTH];
    char line[MAX_LENGTH];
    int commitID = 3;

    while (fgets(line, MAX_LENGTH, file) != NULL){
        sscanf(line, "%s\n", filepath);
        char adr[MAX_LENGTH];
        sprintf(adr, ".neogit/files/%s", filepath);
        if (access(adr, F_OK) != 0){
            buildFileCommitDirectory(filepath);
        }
        copyFile(commitID, filepath);
    }
    fclose(file);

    file = fopen(".neogit/staging", "w");
    if (file == NULL) return 1;
    fclose(file);

}

int buildFileCommitDirectory(char* filepath){
    if (access(".neogit", F_OK) != 0){
        perror("No initialized repository");
        return 1;
    }

    char fileadr[MAX_LENGTH];
    sprintf(fileadr, ".neogit/files/%s", filepath);
    if (mkdir (fileadr, 0755) != 0){
        perror ("Error creating file commit directory");
        return 1;
    }
    return 0;
}

void fileOrDir(char* sth){
    DIR *dir = opendir(".");
    struct dirent* entry;
    if (dir == NULL) {
        printf("Error opening current directory\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && (strcmp(sth, entry->d_name) == 0)){
            addFileToStaging(sth);
            closedir(dir);
            return;
            }
        else if (entry->d_type == DT_DIR && (strcmp(sth, entry->d_name) == 0)){
            addDirToStaging(sth);
            closedir(dir);
            return;
            }
        }
        closedir(dir);
        printf("No such file or directory\n");
        return;
}



int copyFile(int commitID, char* filepath){
    if (access(".neogit", F_OK) != 0){
        perror("No initialized repository");
        return 1;
    }

    FILE* orgfile = fopen(filepath, "r");
    if (orgfile == NULL)
        return 1;

    char copypath[MAX_LENGTH];
    sprintf(copypath, ".neogit/files/%s/%d", filepath, commitID);
    FILE* copyfile = fopen(copypath, "w");
    if (copyfile == NULL)
        return 1;


    char line[MAX_LENGTH];

    while (fgets(line, MAX_LENGTH, orgfile) != NULL){
        fprintf(copyfile, "%s", line);
    }
    fclose(orgfile);
    fclose(copyfile);

    return 0;
}


int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        perror("invalid command");
        return 1;
    }


    if (strcmp(argv[1], "config") == 0){
        if (strcmp (argv[2], "-global") == 0)
        return globalConfig(argv[3], argv[4]);
        else
            return localConfig(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "alias") == 0)
        return addAlias(argv[2], argv[3]);
    else if (strcmp(argv[1], "init") == 0)
        return runInit();
    else if (strcmp(argv[1], "add") == 0){
        if (argc == 2){
            printf("No file or directory specified\n");
            return 1;
        }

        if (strcmp(argv[2], "-f") == 0){
            for (int i = 3; argv[i] != '\0'; i++)
                {
                    printf("%s\n", argv[4]);
                    fileOrDir(argv[i]);
                }
        }
        else
            fileOrDir(argv[2]);
    }

    else if (strcmp(argv[1], "commit") == 0){
        commit();
    }
    else
        return execAlias(argv[1]);

    return 0;

}
