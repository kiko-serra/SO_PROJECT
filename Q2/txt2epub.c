#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//Checking if the file ends with .txt
int checkExtension(char *buf){
    int size = strlen(buf);
    char last[4 + 1];
    strcpy(last, buf + (size - 4));
    if (strcmp(last, ".txt"))
        return 1;
    return 0;
}

//Getting the file name until .txt extension
void getFileName(char *buf){
    buf[strlen(buf) - 4] = '\0';
}

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Usage: %s file1 file2 ... filen\n", argv[0]);
        return EXIT_FAILURE;
    }

    //Starts preparing the zip command->
    char *zipArgumentList[argc + 2];
    zipArgumentList[0] = (char *)malloc((strlen("zip") + 1) * sizeof(char));
    strcpy(zipArgumentList[0], "zip");
    zipArgumentList[1] = (char *)malloc((strlen("ebooks.zip") + 1) * sizeof(char));
    strcpy(zipArgumentList[1], "ebooks.zip");
    zipArgumentList[argc + 1] = NULL;

    for (int i = 1; i < argc; i++){
        if (checkExtension(argv[i])){
            printf("Error: %s is not a .txt file\n", argv[i]);
            return EXIT_FAILURE;
        }
        char *c = (char *)malloc(strlen(argv[i]) + 3);
        strcpy(c, argv[i]);
        getFileName(c);

        char *epubFile = strcat(c, ".epub");
        zipArgumentList[1 + i] = (char *)malloc((strlen(epubFile) + 1) * sizeof(char));
        strcpy(zipArgumentList[1 + i], epubFile);
        free(c);
    }
    //Ends the preparation for the zip command<-

    for (int i = 1; i < argc; i++){
        pid_t pid = fork();
        if (pid == 0){
            printf("[pid%d] converting %s ...\n", getpid(), argv[i]);

            //Changing the file extention
            char *oldFileName = (char *)malloc((strlen(argv[i]) + 2) * sizeof(char)); 
            char *newFileName = (char *)calloc((strlen(argv[i]) + 2), sizeof(char));
            strcpy(oldFileName, argv[i]);
            getFileName(oldFileName);
            strcat(newFileName, strcat(oldFileName, ".epub"));

            //Executing the pandoc command
            if (execlp("pandoc", "pandoc", argv[i], "-o", newFileName, (char *)NULL) == -1)
                perror("execlp():");

            //Let the memory be free!!
            free(oldFileName);
            free(newFileName);
            return EXIT_FAILURE;
        }
        else if (pid == -1){
            perror("fork():");
            return EXIT_FAILURE;
        }
    }
    // Waiting for the childs to end
    for (int i = 1; i < argc; i++)
        if (wait(NULL) == -1){
            perror("wait():");
            return EXIT_FAILURE;
        }

    // Executing the zip command
    if (execvp("zip", zipArgumentList) == -1){
        perror("execvp():");
        return EXIT_FAILURE;
    }
    int zipArgumentList_size = argc + 2;

    //Lets free all space inside zipArgumentList. Since the last position is NULL there's no need to.
    for(int i = 0; i < (zipArgumentList_size - 1); i++)
        free(zipArgumentList[i]);
    return EXIT_SUCCESS;

    //Ahhhhh free memory... I LOVE IT
}
