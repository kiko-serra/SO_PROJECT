#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int checkExtension (char * buf){
    int size = strlen(buf);
    char last [4+1];
    strcpy(last, buf+(size-4));
    if(strcmp(last, ".txt"))
        return 1;
    return 0;
}

void getFileName (char * buf){
    buf[strlen(buf)-4] = '\0';
}

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Usage: %s file1 file2 ... filen\n", argv[0]);
        return 0;
    }
    char zipcommand[10000] = "zip ebooks.zip ";
    for (int i = 1; i < argc; i++){
        if(checkExtension(argv[i])){
            printf("Error: %s is not a .txt file\n", argv[i]);
            return 0;
        }
        char c[10000];
        strcpy(c, argv[i]);
        getFileName(c);
        char *epub_file = strcat(c, ".epub ");
        strcat(zipcommand, epub_file);
    }

    for (int i = 1; i < argc; i++){
        pid_t pid = fork();
        if (pid == 0){
            printf("[pid%d] converting %s ...\n", getpid(), argv[i]);

            //Creating command to use in execlp
            char command[10000] = "pandoc ";
            strcat(command, argv[i]);
            strcat(command, " -o ");
            //!!! mudar strcat para variaveis e fazer strlen +2 
            getFileName(argv[i]);
            strcat(command, strcat(argv[i], ".epub"));

            if (execlp("/bin/sh", "/bin/sh", "-c", command, (char *)NULL) == -1)
                perror("execlp():");
            return 0;
        }
        else if (pid == -1){
            perror("fork():");
            return 0;
        }
    }
    // Waiting for the childs to end
    for (int i = 1; i < argc; i++)
        if (wait(NULL) == -1)
            perror("wait():");

    // Executing the zip command
    if (execlp("/bin/sh", "/bin/sh", "-c", zipcommand, (char *)NULL) == -1)
        perror("execlp():");

    return 0;
}
