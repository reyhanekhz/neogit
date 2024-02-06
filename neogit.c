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

void globalConfig(const char *key, const char *value);
int isStaged(char* filepath);
void localConfig(char* key, char* value);
void runInit();
void addFileToStaging(char* filepath);
void fileOrDir(char* argv[], char* sth);
void resetFile(char* filepath);
void resetUndo();
void addn();
void commit(int argc, char* argv[]);
int updateID(char* branch);
void buildFileCommitDirectory(char* filepath);
void copyFile(int commitID, char* filepath);
void messageShortcut(int argc, char* argv[]);
void logC(int argc, char* argv[]);
void checkoutCommit(int argc, char* argv[]);
void createBranch(int argc, char* argv[]);
int getBranchID(char* branch);
void status(int argc, char* argv[]);
void addAlias(char* alias, char* command);
void execAlias(char* alias);
void listBranch(int argc, char* argv[]);
void checkoutFile(int ID, char* filepath);



void addLocalAlias(char* alias, char* command){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    FILE* file = fopen (".neogit/aliases", "a");
    if (file == NULL){
        perror ("Error opening aliases file");
        return;
    }

    fprintf(file, "%s : %s\n", alias, command);
    fclose(file);
    return;

}

void addGlobalAlias(char* alias, char* command){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    FILE* file = fopen ("/home/reyhane/.neogitGlobalSettings/globalAliases", "a");
    if (file == NULL){
        printf ("Error opening aliases file\n");
        return;
    }

    fprintf(file, "%s : %s\n", alias, command);
    fclose(file);

}

void execAlias(char* alias){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    char line[MAX_LENGTH];
    char orgAlias[MAX_LENGTH];
    char command[MAX_LENGTH];

    FILE* file = fopen(".neogit/aliases", "r");
    while ( fgets(line, MAX_LENGTH, file) != NULL){
        int length = strlen(line);
        if (length > 0 && line[length - 1] == '\n')
            line[length - 1] = '\0';
        sscanf(line, "%s : %[^\n]s", orgAlias, command);

        if (strcmp(orgAlias, alias) == 0){
            system(command);
            return;
        }
    }

    printf ("Invalid alias\n");

}

void globalConfig(const char *key, const char *value) {
    if (strcmp(key, "user.name") == 0)
    {
        FILE* file = fopen("/home/reyhane/.neogitGlobalSettings/globalConfig/globalUsername", "w");
        if (file == NULL) {
            printf("Error opening username global config file\n");
            return;
        }

        fprintf(file, "username : %s", value);
        fclose(file);
    }

    else if(strcmp(key, "user.email") == 0){
        FILE* file = fopen("/home/reyhane/.neogitGlobalSettings/globalConfig/globalUseremail", "w");
        if (file == NULL) {
            printf("error opening useremail global config file\n");
            return;
        }

        fprintf(file, "useremail : %s", value);
        fclose(file);
    }

}

int isStaged(char* filepath){
    FILE* file = fopen (".neogit/staging", "r");

    char line[MAX_LENGTH];
    while (fgets(line, MAX_LENGTH, file) != NULL){
        int length = strlen(line);
        if (length > 0 && line[length - 1] == '\n')
            line[length - 1] = '\0';

        if (strcmp(line, filepath) == 0)
            return 1;
    }

    fclose(file);
    return 0;
}

void localConfig(char* key, char* value){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    if (strcmp(key, "user.name") == 0)
    {
        FILE* file = fopen (".neogit/localConfig/localUsername", "w");
        if (file == NULL){
            printf("Error opening local username file\n");
            return;
        }
        fprintf(file, "local username : %s", value);
        fclose (file);
    }
    if (strcmp(key, "user.email") == 0)
    {
        FILE* file = fopen (".neogit/localConfig/localUseremail", "w");
        if (file == NULL){
            printf("Error opening local useremail file\n");
            return;
        }
        fprintf(file, "local useremail : %s", value);
        fclose (file);
    }

}

void runInit() {
    char parentdir[MAX_LENGTH];
    char currentdir[MAX_LENGTH];
    if (getcwd(currentdir, MAX_LENGTH) == NULL)
        return;


    do {
        if (access(".neogit", F_OK) == 0){
        printf("Neogit repository already exists\n");
        return;
        }

        if (chdir("..") != 0)
            return;

        if (getcwd(parentdir, MAX_LENGTH) == NULL)
            return;


    } while (strcmp(parentdir, "/") != 0);

    if (chdir(currentdir) != 0)
        return;

    if (mkdir (".neogit", 0755) != 0){
        printf("Error creating Neogit directory\n");
        return;
    }

    if (access("/home/reyhane/.neogitGlobalSettings", F_OK) != 0){
        if (mkdir("/home/reyhane/.neogitGlobalSettings", 0755) != 0){
            perror ("Error creating global settings directory");
            return;
        }
        else {
            if (mkdir ("/home/reyhane/.neogitGlobalSettings/globalConfig", 0755) ==0){
                FILE* file = fopen("/home/reyhane/.neogitGlobalSettings/globalConfig/globalUsername", "w");
                fclose (file);
                file = fopen("/home/reyhane/.neogitGlobalSettings/globalConfig/globalUseremail", "w");
                fclose(file);
                file = fopen("/home/reyhane/.neogitGlobalSettings/globalALiases", "w");
                fclose(file);
            }
            else{
                perror ("Error creating global config directory");
                return;
            }
        }
    }

    if(mkdir(".neogit/localConfig", 0755) != 0){
        printf("Error creating directory\n");
        return;
    }

    if(mkdir(".neogit/commits", 0755) != 0){
        printf("Error creating commit directory\n");
        return;
    }

    if(mkdir(".neogit/files", 0755) != 0){
        printf("Error creating files directory\n");
        return;
    }

    if(mkdir(".neogit/branchID", 0755) != 0){
        printf("Error creating directory\n");
        return;
    }

    FILE* file = fopen (".neogit/trackedFiles", "w");
    fclose(file);
    file = fopen (".neogit/staging", "w");
    fclose(file);
    file = fopen (".neogit/latestStaged", "w");
    fclose(file);
    file = fopen (".neogit/localConfig/localUsername", "w");
    fclose(file);
    file = fopen (".neogit/localConfig/localUseremail", "w");
    fclose(file);
    file = fopen (".neogit/aliases", "w");
    fclose(file);
    file = fopen (".neogit/messageShortcuts", "w");
    fclose (file);
    file = fopen (".neogit/HEAD", "w");
    fprintf(file, "master");
    fclose(file);
    file = fopen (".neogit/commitID", "w");
    fprintf(file, "100");
    fclose(file);
    file = fopen(".neogit/branchID/master", "w");
    fprintf(file, "100");
    fclose(file);


    printf("Empty Neogit repository initialized in %s\n", currentdir);

}

void addFileToStaging(char* filepath) {
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    if (isStaged(filepath)){
        printf("File is already stgaed\n");
        return;
    }

    FILE* file = fopen (".neogit/staging", "a");
    if (file == NULL)
        return;

    fprintf(file, "%s\n", filepath);

    printf("%s : Added to staging area\n", filepath);
    fclose (file);
}

int updateID(char* branch){
    FILE* file = fopen (".neogit/commitID", "r");
    if (file == NULL)
        return -1;

    int ID;
    char string[MAX_LENGTH];
    fscanf(file, "%s", string);
    ID = atoi(string);
    ID ++;
    fclose (file);

    file = fopen (".neogit/commitID", "w");
    fprintf(file, "%d", ID);
    fclose(file);

    char adr[MAX_LENGTH];
    sprintf(adr, ".neogit/branchID/%s", branch);
    file = fopen (adr, "w");
    fprintf(file, "%d", ID);
    fclose(file);

    return ID;
}

void commit(int argc, char* argv[]){
    if (access(".neogit", F_OK) != 0) {
        printf("No initialized repository\n");
        return;
    }

    if (argc<3)
    {
        printf("No Commit message\n");
        return;
    }

    char message[MAX_LENGTH];
    if (strcmp(argv[2], "-m") == 0)
        strcpy(message, argv[3]);

    else if (strcmp(argv[2], "-s") == 0){
        int flag = 0;
        char buffer[MAX_LENGTH];
        FILE* fp = fopen (".neogit/messageShortcuts", "r");
        while (fgets (buffer, MAX_LENGTH, fp) != NULL){
            int l = strlen(buffer);
            if (l > 0 && buffer[l - 1] == '\n')
                buffer[l - 1] = '\0';

            char shortcut[MAX_LENGTH];
            char m[MAX_LENGTH];
            sscanf(buffer, "%s : %[^\n]s", shortcut, m);
            if(strcmp(argv[3], shortcut) == 0){
                strcpy(message, m);
                flag =1;
                break;
            }
        }
        fclose (fp);
        if (flag == 0){
            printf("Invalid shortcut\n");
            return;
        }
    }

    int size = strlen(message);
    if (size >72){
        printf("Commit message too long\n");
        return;
    }

    FILE* file = fopen (".neogit/HEAD", "r");
    char branch[MAX_LENGTH];
    fscanf(file, "%s", branch);
    fclose(file);

    int commitID = updateID(branch);


    file = fopen(".neogit/staging", "r");
    if (file == NULL){
        printf("Error opening file\n");
        return;
    }

    char line[MAX_LENGTH];
    int filecount =0;

    while (fgets(line, MAX_LENGTH, file) != NULL) {
        int length = strlen(line);
        if (length > 0 && line[length - 1] == '\n')
            line[length - 1] = '\0';

        char adr[1500];
        sprintf(adr, ".neogit/files/%s", line);
        if (access(adr, F_OK) != 0)
            buildFileCommitDirectory(line);

        copyFile(commitID, line);
        filecount ++;
    }
    fclose(file);


    file = fopen(".neogit/staging", "w");
    if (file == NULL) return;
    fclose(file);

    file = fopen (".neogit/localConfig/localUsername", "r");
    if (file == NULL){
        printf("Error opening file\n");
        return;
    }

    char author[MAX_LENGTH];
    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
        fclose(file);
        file = fopen ("/home/reyhane/.neogitGlobalSettings/globalConfig/globalUsername", "r");
        if (file == NULL){
            printf("Error opening file\n");
            return;
        }

        fscanf(file, "username : %[^\n]s", author);
        fclose(file);
    }
    else{
        fscanf(file, "local username : %[^\n]s", author);
        fclose(file);
    }

    file = fopen (".neogit/localConfig/localUseremail", "r");
    if (file == NULL){
        printf("Error opening file\n");
        return;
    }

    char email[MAX_LENGTH];
    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
        fclose(file);
        file = fopen ("/home/reyhane/.neogitGlobalSettings/globalConfig/globalUseremail", "r");
        if (file == NULL){
            printf("Error opening file\n");
            return;
        }

        fscanf(file, "useremail : %[^\n]s", email);
        fclose(file);
    }
    else{
        fscanf(file, "local useremail : %[^\n]s", email);
        fclose(file);
    }

    time_t current_time;
    struct tm* time_info;

    time(&current_time);
    time_info = localtime(&current_time);

    printf("On branch : %s\nCommit ID : %d\nCommit Message : %s\nFiles commited: %d\nAuthor: %s\n%s", branch, commitID, message, filecount, author, asctime(time_info));

    char adr[MAX_LENGTH];
    sprintf(adr, ".neogit/commits/%d", commitID);
    file = fopen (adr, "w");
    fprintf(file, "On branch : %s\nCommit ID : %d\nCommit Message : %s\nFiles commited: %d\nAuthor: %s\n%s", branch, commitID, message, filecount, author, asctime(time_info));
    fclose(file);

}

void buildFileCommitDirectory(char* filepath){
    char fileadr[MAX_LENGTH];
    sprintf(fileadr, ".neogit/files/%s", filepath);
    if (mkdir (fileadr, 0755) != 0){
        perror ("Error creating file commit directory");
        return;
    }
}

void fileOrDir(char* argv[], char* sth){
    DIR *dir = opendir(".");
    struct dirent* entry;
    if (dir == NULL) {
        printf("Error opening current directory\n");
        return;
    }

    FILE* fp = fopen (".neogit/latestStaged", "a");


    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && (strcmp(sth, entry->d_name) == 0)){
            addFileToStaging(sth);
            fprintf(fp , "%s\n", sth);
            closedir(dir);
            fclose(fp);
            return;
        }

        else if (entry->d_type == DT_DIR && (strcmp(sth, entry->d_name) == 0)){
            DIR* direc = opendir(entry->d_name);
            struct dirent* file;

            while ((file = readdir(direc)) != NULL){
                if (file->d_type == DT_REG){
                    addFileToStaging(file->d_name);
                    fprintf(fp , "%s\n", sth);
                }
            }
            closedir(direc);
            closedir(dir);
            fclose(fp);
            return;
        }
    }

    fclose(fp);
    closedir(dir);
    printf("No such file or directory\n");

}

void copyFile(int commitID, char* filepath){
    FILE* orgfile = fopen(filepath, "r");
    if (orgfile == NULL)
        return;

    char copypath[MAX_LENGTH];
    sprintf(copypath, ".neogit/files/%s/%d", filepath, commitID);
    FILE* copyfile = fopen(copypath, "w");
    if (copyfile == NULL)
        return;


    char line[MAX_LENGTH];

    while (fgets(line, MAX_LENGTH, orgfile) != NULL)
        fprintf(copyfile, "%s", line);

    fclose(orgfile);
    fclose(copyfile);

}

void resetFile(char* filepath){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    if (!isStaged(filepath)){
        printf("File is not staged already\n");
        return;
    }

    FILE* file = fopen(".neogit/staging", "r");
    FILE* tempfile= fopen (".neogit/temp", "w");
    char line[MAX_LENGTH];

    if (file == NULL || tempfile == NULL) {
        printf("Error opening file\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int length = strlen(line);
        if (length > 0 && line[length - 1] == '\n')
            line[length - 1] = '\0';

        if (strcmp(line, filepath) != 0)
            fprintf(tempfile, "%s\n", line);
    }

    fclose(file);
    fclose(tempfile);

    system("rm .neogit/staging");
    system("cp .neogit/temp .neogit/staging");

    return;

}

void resetUndo(){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    FILE* file = fopen (".neogit/latestStaged", "r");
    if (file == NULL){
        printf("Error opening file\n");
        return;
    }

    char line[MAX_LENGTH];
    fgets(line, MAX_LENGTH, file);
    line [strcspn(line, "\n")] = '\0';
    resetFile(line);

    fclose(file);
    file = fopen (".neogit/latestStaged", "w");
    fclose(file);
}

void addn(){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    DIR *dir = opendir(".");
    struct dirent* entry;
    if (dir == NULL) {
        printf("Error opening current directory\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG){
            if (isStaged(entry->d_name))
                printf("%s: staged\n", entry->d_name);
            else
                printf("%s: not staged\n", entry->d_name);
        }

        else if (entry->d_type == DT_DIR ){
            DIR* direc = opendir (entry->d_name);
            struct dirent* files;
            if (direc == NULL) {
                printf("Error opening current directory\n");
                return;
            }
            while((files = readdir(direc)) != NULL){
                if (files->d_type == DT_REG){
                    if (isStaged(files->d_name))
                        printf("%s: staged\n", files->d_name);
                    else
                        printf("%s: not staged\n", files->d_name);
                }
            }
            closedir(direc);
        }

    }
    closedir(dir);

}

void messageShortcut(int argc, char* argv[]){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    if (argc < 5){
        printf("Enter a shortcut-name\n");
        return;
    }

    char message[MAX_LENGTH];
    char shortcut[MAX_LENGTH];
    strcpy(message, argv[3]);
    strcpy(shortcut, argv[4]);

    FILE* file = fopen(".neogit/messageshortcuts", "w");
    if (file == NULL){
        printf("Error opening file\n");
        return;
    }

    fprintf(file, "%s : %s\n", shortcut, message);
    fclose(file);
}

void checkoutCommit(int argc, char* argv[]){
    int ID;
    ID = atoi(argv[2]);

    DIR* dir = opendir(".");
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL){
        if (entry->d_type == DT_REG)
            checkoutFile(ID, entry->d_name);

        else if (entry->d_type == DT_DIR){
            DIR* direc = opendir(entry->d_name);
            struct dirent* file;

            while ((file = readdir(direc)) != NULL){
                if (file->d_type == DT_REG)
                    checkoutFile(ID, file->d_name);
            }
            closedir(direc);
        }
    }

    closedir(dir);
}

void checkoutFile(int ID, char* filepath){
    int maxid = -1;
    char adr[MAX_LENGTH];
    sprintf(adr, ".neogit/files/%s", filepath);
    DIR *dir = opendir(adr);
    if (dir == NULL)
        return;
    struct dirent *entry;

    while((entry = readdir(dir)) != NULL) {
        int tmp = atoi(entry->d_name);
        if (maxid < tmp && tmp <= ID)
            maxid = tmp;
    }

    closedir(dir);
    char buffer[MAX_LENGTH];

    if (maxid == -1) {
        sprintf(buffer, "rm %s", filepath);
        system(buffer);
        return;
    }

    sprintf(buffer, ".neogit/files/%s/%d", filepath, maxid);

    FILE* commitfile = fopen(buffer, "r");
    if (commitfile == NULL)
        return;

    FILE* orgfile = fopen(filepath, "w");
    if (orgfile == NULL)
        return;

    char line[MAX_LENGTH];
    while (fgets(line, MAX_LENGTH, commitfile) != NULL)
        fprintf(orgfile, "%s", line);

    fclose(orgfile);
    fclose(commitfile);
}

void createBranch(int argc, char* argv[]){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    char adr[MAX_LENGTH];
    sprintf(adr, ".neogit/branchID/%s", argv[2]);
    FILE* file = fopen (adr, "w");
    if (file == NULL){
        printf("Error opening file\n");
        return;
    }

    fprintf(file, "%d", getBranchID("master"));
    fclose(file);

    printf("%s : New branch created at commit ID : %d\n", argv[2], getBranchID("master"));
}

int getBranchID(char* branch){
    char adr[MAX_LENGTH];
    sprintf(adr, ".neogit/branchID/%s", branch);
    FILE* file = fopen (adr, "r");
    if (file == NULL){
        printf("Branch doesn't exist\n");
        return -1;
    }

    int ID;
    char string[MAX_LENGTH];
    fscanf(file , "%s", string);
    ID = atoi(string);
    fclose(file);

    return ID;
}

void checkoutBranch(int argc, char* argv[]){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    char branch[MAX_LENGTH];
    if (strcmp(argv[2], "HEAD") == 0){
        FILE* file = fopen (".neogit/HEAD", "r");
        fscanf(file, "%s", branch);
    }
    else
        strcpy(branch, argv[2]);

    char adr[MAX_LENGTH];
    sprintf(adr, ".neogit/branchID/%s", argv[2]);
    FILE* file = fopen (adr, "r");
    if (file == NULL){
        printf("Branch doesn't exist\n");
        return;
    }
    fclose(file);

    file = fopen (".neogit/HEAD", "w");
    fprintf(file, "%s", argv[2]);

    int ID = getBranchID(argv[2]);
    DIR* dir = opendir(".");
    if (dir == NULL)
        return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL){
        if (entry->d_type == DT_REG)
            checkoutFile(ID, entry->d_name);

        else if (entry->d_type == DT_DIR){
            DIR* direc = opendir(entry->d_name);
            struct dirent* file;

            while ((file = readdir(direc)) != NULL){
                if (file->d_type == DT_REG)
                    checkoutFile(ID, file->d_name);
            }
            closedir(direc);
        }
    }

    closedir(dir);
}

void logC(int argc, char* argv[]){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    int tempid = 101;
    int currentid;
    FILE* file = fopen (".neogit/commitID", "r");
    if (file == NULL){
        printf("Error opening file\n");
        return;
    }

    char temp[MAX_LENGTH];
    fscanf(file, "%s", temp);
    currentid = atoi(temp);
    fclose(file);

    while(tempid <= currentid){
        char adr[MAX_LENGTH];
        sprintf(adr, ".neogit/commits/%d", tempid);
        file = fopen (adr, "r");
        if (file == NULL){
            printf("Error opening file\n");
            return;
        }

        char line[MAX_LENGTH];
        if (argc >2){
            if (strcmp( argv[2], "-branch") == 0){
                fgets(line, MAX_LENGTH, file);
                char branch[MAX_LENGTH];
                sscanf(line, "On branch : %s\n", branch);

                if (strcmp(argv[3], branch) == 0){
                    rewind(file);
                    while(fgets(line, MAX_LENGTH, file) != NULL)
                        printf("%s", line);
                    printf("\n");
                }
            }
        }
        else {
            while(fgets(line, MAX_LENGTH, file) != NULL)
                printf("%s", line);
            printf("\n");
        }

        fclose(file);
        tempid++;
    }

}

void listBranch(int argc, char* argv[]){
    DIR* dir = opendir(".neogit/branchID");
    if (dir == NULL){
        printf("Error opening directory\n");
        return;
    }

    struct dirent* entry;
    printf("Branches:\n");
    while ((entry = readdir (dir)) != NULL)
        printf("%s\n", entry->d_name);

    closedir(dir);
}


int main(int argc, char* argv[]){
    if (argc == 1)
        printf("invalid command\n");

    if (strcmp(argv[1], "config") == 0){
        if (strcmp (argv[2], "-global") == 0){
            if (strcmp(argv[3], "user.name") == 0 || strcmp(argv[3], "user.email") == 0)
                globalConfig(argv[3], argv[4]);
            else{
                char alias[MAX_LENGTH];
                sscanf(argv[3], "alias.%s", alias);
                addGlobalAlias(alias, argv[4]);
            }
        }
        else {
            if (strcmp(argv[2], "user.name") == 0 || strcmp(argv[2], "user.email") == 0)
                localConfig(argv[2], argv[3]);
            else{
                char alias[MAX_LENGTH];
                sscanf(argv[2], "alias.%s", alias);
                addLocalAlias(alias, argv[3]);
            }
        }
    }

    else if (strcmp(argv[1], "init") == 0)
        runInit();

    else if (strcmp(argv[1], "add") == 0){
        if (argc == 2){
            printf("No file or directory specified\n");
            return 1;
        }

        FILE* file = fopen (".neogit/latestStaged", "w");
        fclose(file);

        if (strcmp(argv[2], "-f") == 0) {
            for (int i = 3; i<argc ; i++){
                printf("%s: ", argv[i]);
                fileOrDir(argv, argv[i]);
            }
        }
        else if (strcmp(argv[2], "-n") == 0)
            addn();

        else
            fileOrDir(argv, argv[2]);
    }
    else if (strcmp(argv[1], "reset") == 0){
        if (strcmp(argv[2], "-undo") == 0)
            resetUndo();
        else resetFile(argv[2]);
    }
    else if (strcmp(argv[1], "commit") == 0)
        commit(argc, argv);
    else if (strcmp(argv[1], "set") == 0)
        messageShortcut(argc, argv);

    else if (strcmp(argv[1], "log") == 0)
        logC(argc, argv);

    else if (strcmp(argv[1], "checkout") == 0){
        DIR* dir = opendir(".neogit/branchID");
        struct dirent* entry;
        int flag =0;
        while ((entry = readdir(dir)) != NULL){
            if (strcmp(argv[2], entry->d_name) == 0){
                checkoutBranch(argc, argv);
                flag = 1;
                break;
            }
        }
        if (strcmp(argv[2], "HEAD") == 0){
            checkoutBranch(argc, argv);
            flag == 1;
        }
        if (flag == 0)
            checkoutCommit(argc, argv);
    }


    else if (strcmp(argv[1], "branch") == 0){
        if (argc == 3)
            createBranch(argc, argv);
        else if (argc == 2)
            listBranch(argc, argv);
    }

    else
        execAlias(argv[1]);

    return 0;

}
