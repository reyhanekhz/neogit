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

int globalConfig(const char *key, const char *value);
int isStaged(char* filepath);
int localConfig(char* key, char* value);
int runInit();
void addDirToStaging(char argv[2], char* directory);
void addFileToStaging(char argv[2], char* filepath);
void fileOrDir(char argv[2], char* sth);
void resetFile(char* filepath);
void resetUndo();
void addn();
void commit(int argc, char* argv[]);
int incrementID();
void buildFileCommitDirectory(char* filepath);
void copyFile(int commitID, char* filepath);
void messageShortcut(int argc, char* argv[]);
void log(int argc, char* argv[]);
void checkoutCommit(int argc, char* argv[]);



/*int addAlias(char* alias, char* command){
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
*/

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
        printf("Neogit repository already exists\n");
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
        printf("Error creating Neogit directory\n");
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
    fprintf(file, "100");
    fclose(file);


    printf("Empty Neogit repository initialized in %s\n", currentdir);
    return 0;

}

/*void addDirToStaging(char argv[2], char* directory){
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
            if (isStaged(entry->d_name) == 1){
                printf("File already added to staging area\n");
                fclose(file);
                return;
            }

            fclose(file);

            file = fopen (".neogit/staging", "a");
            if (strcmp(argv[2], "-f") == 0)
                FILE* f = fopen (".neogit/latestStaged", "a");
            else
                FILE* f = fopen (".neogit/latestStaged", "w");
            fprintf(file, "%s\n", entry->d_name);
            fprintf(f, "%s\n", entry->d_name);
            fclose(file);
            fclose(f);
        }
    }

    closedir(dir);

}

void addFileToStaging(char argv[2], char* filepath){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    if (isStaged(filepath)){
        printf("File already added to staging area\n");
        return;
    }

    FILE* file = fopen (".neogit/staging", "a");
    if (strcmp(argv[2], "-f") == 0)
        FILE* f = fopen (".neogit/latestStaged", "a");
    else
        FILE* f = fopen (".neogit/latestStaged", "w");


    if (file == NULL || f == NULL)
        return;

    fprintf(file, "%s\n", filepath);
    fprintf(f, "%s\n", filepath);

    printf("Added to staging area\n");
    fclose (file);
    fclose(f);

}*/

int incrementID(){
    FILE* file = fopen (".neogit/commitID", "r");
    char idstring[MAX_LENGTH];
    int ID;
    fscanf(file, "%s", idstring);
    ID = atoi(idstring);
    fclose(file);

    file = fopen (".neogit/commitID", "w");
    ID++;
    sprintf(idstring, "%d", ID);
    fprintf(file, "%s", idstring);
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
        FILE* file = fopen (".neogit/messageShortcuts", "r");
        while (fgets (buffer, MAX_LENGTH, file) != NULL){
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
        fclose (file);
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
    int commitID = incrementID();

    FILE* file = fopen(".neogit/staging", "r");
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

        char adr[MAX_LENGTH];
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
        if (file == NULL)
            return;

        fscanf(file, "username : %[^\n]s", author);
        fclose(file);
    }
    else{
        fscanf(file, "username : %[^\n]s", author);
        fclose(file);
    }

    time_t current_time;
    struct tm* time_info;

    time(&current_time);
    time_info = localtime(&current_time);

    printf("Commit done successfully\nCommit ID : %d\nCommit Message : %s\nFiles commited: %d\nAuthor: %s\n%s", commitID, message, filecount, author, asctime(time_info));

    char adr[MAX_LENGTH];
    sprintf(adr, ".neogit/commits/%d", commitID);
    file = fopen (adr, "w");
    fprintf(file, "On branch : MASTER\nCommit ID : %d\nCommit Message : %s\nFiles commited: %d\nAuthor: %s\n%s", commitID, message, filecount, author, asctime(time_info));
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

/*void fileOrDir(char argv[2], char* sth){
    DIR *dir = opendir(".");
    struct dirent* entry;
    if (dir == NULL) {
        printf("Error opening current directory\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && (strcmp(sth, entry->d_name) == 0)){
            addFileToStaging(char argv[2], sth);
            closedir(dir);
            return;
            }
        else if (entry->d_type == DT_DIR && (strcmp(sth, entry->d_name) == 0)){
            addDirToStaging(char argv[2], sth);
            closedir(dir);
            return;
            }
        }
    closedir(dir);
    printf("No such file or directory\n");
    return;
}*/

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
        perror("No initialized repository");
        return;
    }

    if (!isStaged(filepath)){
        printf("File is not staged already");
        return;
    }

    FILE* file = fopen(".neogit/staging", "r");
    FILE* tempfile= fopen (".neogit/temp", "w");
    char line[1024];

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

void log(int argc, char* argv[]){
    if (access(".neogit", F_OK) != 0){
        printf("No initialized repository\n");
        return;
    }

    int tempid = 104;
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

void checkoutCommit(int argc, char* argv[]){
    int ID;
    ID = atoi(argv[2]);

    DIR* dir = opendir(".");
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL){
        if (entry->d_type == DT_REG)
            checkoutFile(ID, entry->d_name);
    }

}

void checkoutFile(int ID, char* filepath){
    int maxid = -1;
    char adr[MAX_LENGTH];
    sprintf(adr, ".neogit/files/%s", filepath);
    DIR *dir = opendir(adr);
    if (dir == NULL) return 1;
    struct dirent *entry;

    while((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            int tmp = atoi(entry->d_name);
            if (tmp > maxid && tmp <= ID)
                maxid = tmp;
        }
    }
    closedir(dir);
    char buffer[MAX_LENGTH];

    if (maxid == -1){
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

   /* else if (strcmp(argv[1], "alias") == 0)
        return addAlias(argv[2], argv[3]);
*/
    else if (strcmp(argv[1], "init") == 0)
        return runInit();

    else if (strcmp(argv[1], "add") == 0){
        if (argc == 2){
            printf("No file or directory specified\n");
            return 1;
        }

        /*if (strcmp(argv[2], "-f") == 0) {
            for (int i = 3; i<argc ; i++){
                printf("%s: ", argv[i]);
                fileOrDir(char argv[2], argv[i]);
            }
        }*/
        else if (strcmp(argv[2], "-n") == 0)
            addn();

        /*else
            fileOrDir(argv[2]);*/
    }
    else if (strcmp(argv[1], "reset") == 0)
        resetUndo();
    else if (strcmp(argv[1], "commit") == 0)
        commit(argc, argv);
    else if (strcmp(argv[1], "set") == 0)
        messageShortcut(argc, argv);
    else if (strcmp(argv[1], "log") == 0)
        log(argc, argv);
    else if (strcmp(argv[1], "checkout") == 0)
        checkoutCommit(argc, argv);


         /*else
        return execAlias(argv[1]);
    */return 0;

}
