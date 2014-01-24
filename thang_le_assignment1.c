/*
 * Assignment 1
 * Thang Le
 * CSE 3320
 */

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

// define constraints
#define MAX_NAME 2048
#define MAX_FILES 1024

// function prototypes
void printDirectory();
void printTime();
void freeMem(char **, int);
char **getFilesInDir(unsigned char , int *);
void printMenu();
char *findFile(unsigned char);


// print the current working directory.
void printDirectory() {
    char s[256];

    getcwd(s, 200);
    printf("Current Directory: %s\n", s);
}

// print the current time.
void printTime() {
    time_t t;

    t = time(NULL);
    printf( "Current time: %s\n", ctime( &t ));
}

// free memory for a char **
void freeMem(char **files, int size) {
    int i;
    for (i = 0; i < size; i++) {
        free(files[i]);
    }

    free(files);
}

// record file names of a specified type and also count how
// many files of that type are in the current working directory.
char **getFilesInDir(unsigned char type, int *fileCounter) {
    DIR *d;
    struct dirent *de;
    char **files = NULL;
    int counter = 0;

    d = opendir(".");   //open current working directory.

    // count the number of files of certain type and record the names.
    int i = 0;
    while ((de = readdir(d))) {

        // check and handle the constraint of max characters in a name for files and directories.
        if (strlen(de->d_name) > MAX_NAME) {
            printf("ERROR! At less one file in the current directory exceed the max \n"
                    "of %d characters allowed as a name.\n\nExiting program...\n\n", MAX_NAME);
            exit(1);
        }

        if (de->d_type == type) {
            counter++;
            files = realloc(files, counter * sizeof(char *));
            files[i] = malloc(strlen(de->d_name) + 1);
            strcpy(files[i], de->d_name);
            i++;
        }
    }

    closedir(d);
    *fileCounter = counter;

    return files;
}

// display the menu which includes the CWD, current time, files, directories,
// and operations that the user can call.
void printMenu() {
    char **files;
    char **directories;
    int fileCounter, dirCounter = 0;
    int i;

    printDirectory();
    printTime();

    files = getFilesInDir(DT_REG, &fileCounter);
    directories = getFilesInDir(DT_DIR, &dirCounter);

    // check and handle the constraint of max number of files and directories in a directory.
    if ((fileCounter + dirCounter) > MAX_FILES) {
        printf("ERROR! Number of files and directories in the\n"
                "current directory exceed the max of %d.\n\nExiting program...\n\n", MAX_FILES);
        exit(1);
    }

    if (fileCounter == 0) {
        printf("Files:        There is no file in current directory.\n"); // handle 0 file in a directory.
    } else {
        // print files in current working director
         printf( "\n====================================\n"
                "               Files"
                "\n====================================\n");
        i = 0;
        while (i < fileCounter) {
            printf("%d. %s\n", i, files[i]);
            i++;

            if ((i % 10) == 0) { // set to print 8 items at a time.
                printf("\nEnter any key for next, 'p' for previous, or 's' to skip: ");

                char c = getchar();
                while (getchar() != '\n' && c != EOF);    // flush input buffer.
                switch(c) {
                    case 'p': // previous
                        i -= 20;

                        // protection against accessing the array index
                        // less then 0 use by the previous command.
                        if (i < 0) { i = 0; }
                        break;
                    case 's': // skip to menu operations
                        i = fileCounter;
                        break;
                }
            }
        }
    }

    printf("\nFinished listing files. Moving on to listing directories.\nPress Enter to continue.");
    while (getchar() != '\n');

    if (dirCounter == 0) {
        printf("\nDirectory:     There is no sub-directory in current directory.");
    } else {
        // print sub-directories in current working directory
        printf( "\n====================================\n"
                "             Directories"
                "\n====================================\n");
        i = 0;
        while (i < dirCounter) {
            printf("%d. %s\n", i, directories[i]);
            i++;

            if ((i % 10) == 0) { // set to print 10 items at a time.
                printf("\nEnter any key for next, 'p' for previous, or 's' to skip: ");

                char c = getchar();
                while (getchar() != '\n' && c != EOF);    // flush input buffer.
                switch(c) {
                    case 'p': // previous
                        i -= 20;

                        // protection against accessing the array index
                        // less then 0 use by the "previous" command.
                        if (i < 0) { i = 0; }
                        break;
                    case 's': // skip to menu operations
                        i = dirCounter;
                        break;
                }
            }
        }
    }

    printf("------------------------------------\n");
    printf( "\n"
            "Operation:     'e'  Edit\n"
            "               'r'  Run\n"
            "               'c'  Change directory\n"
            "               'p'  Print directory\n"
            "               'q'  Quit\n"
            "\n");
    printf("------------------------------------\n");

    freeMem(files, fileCounter);
    freeMem(directories, dirCounter);
}

// Get user's input and check if the user inputed a number to select a file.
// If the input is a number, try to match it. However, if there is no match or the input
// is not a number, return what was inputed.
char *findFile(unsigned char type) {
    char *input = malloc(sizeof(char *));
    char buffer[256];
    int i ,counter;
    char **files;

    files = getFilesInDir(type, &counter);

    if (fgets(buffer, sizeof(buffer), stdin)) { // get user's input
        if (1 == sscanf(buffer, "%d", &i)) {  // check to find file by file number.
            if (i < counter) {     // if so, check if within range of the number of files in the directory.
                strcpy(input, files[i]);
                freeMem(files, counter);
                return input;
            }
        }
        strtok(buffer, "\n"); // remove trailing newline.
        strcpy(input, buffer);
    }

    freeMem(files, counter);
    return input;
}

int main(void)
{
    int c, l;
    char *s;
    char cmd[256];
    const char EDITOR[] = "nano ";

    system("clear");
    printMenu();
    printf("Choose an Operation:> ");

    while (1) {
        c = getchar();
        while (getchar() != '\n' && c != EOF);    // flush input buffer.

        switch (c) {
            case 'q': exit(0);      // quit program

            case 'c': printf("Change To?: ");
                      s = findFile(DT_DIR);
                      strcpy(cmd, s);
                      if (chdir(cmd) == -1) {
                          printf("No such directory.Try again.\n\n");
                      } else {
                          system("clear");
                          printMenu();
                      }

                      printf("Choose an Operation:> ");
                      break;

            case 'e': printf("Edit what?: ");
                      s = findFile(DT_REG);
                      strcpy(cmd, EDITOR);
                      strcat(cmd, s);
                      system(cmd);
                      printf("Choose an Operation:> ");
                      break;

            case 'r': printf("Run what?: ");
                      s = findFile(DT_REG);
                      strcpy(cmd, "./");
                      strcat(cmd, s);
                      system(cmd);
                      printf("Choose an Operation:> ");
                      break;

            case 'p': system("clear");
                      printMenu();
                      printf("Choose an Operation:> ");

            default: printf("Invalid operation! Try again...\n");
                      printf("Choose an Operation:> ");
                      continue;
        }
        free(s);
    }

    return 0;
}
