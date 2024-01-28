#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


#define MAXSIZE 1024

int run_init(int argc, char* const argv[]);


int run_init(int argc, char* const argv[])
{
    //storing the current directory
    char cwd[MAXSIZE];
    if ( getcwd (cwd, MAXSIZE) == NULL ) return 1;
    //defining a boolean value to check weather the .neogit directory already exists or not
    bool exists = false;
    //checking if current directory or its parent directories contain a ".neogit" directory until we get to the root directory
    char temp_cwd[MAXSIZE];
    struct dirent *entry;

    do {
        // opening current directory
        DIR* dir = opendir (".");
        if ( dir == NULL ) {
            perror ("Error opening directory");
            return 1;
        }
        //checking the existence of .neogit directory
        while ( (entry = readdir (dir)) != NULL) {
            if ((entry->d_type== DT_DIR) && (strcmp (entry->d_name, ".neogit") == 0))
                exists = true;
        }
        //close current directory
        if (closedir(dir) != 0){
            perror ("Error closing the directory");
            return 1;
        }
        // update current working directory
        if (getcwd(temp_cwd, MAXSIZE) == NULL )
            return 1;
        //changing directory to parent if it's not root

        if (strcmp(temp_cwd, "/") != 0) {
            if (chdir("..") != 0) {
                perror ("Error changing directory to parent");
            }
        }
    } while (strcmp(temp_cwd, "/") != 0); //the loop shall continue until the parent is root(we don't check root)

    // return to the initial directory
    if (chdir(cwd) != 0) return 1;

    if (!exists) {
        if (mkdir(".neogit", 0755) != 0) return 1;
        return create_config("reyhane", "reyhaneh.kh1384@gmail.com");
    } else {
        perror("neogit repository has already initialized");
    }
    return 0;
}






int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        perror ("Invalid command");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0)
        return run_init(argc, argv);


    return 0;
}