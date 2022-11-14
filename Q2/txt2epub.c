#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Usage: %s file1 file2 ... filen\n", argv[0]);
        return 0;
    }
    char zipcommand[10000] = "zip ebooks.zip ";
    for (int i = 1; i < argc; i++){
        char c[10000];
        strcpy(c, argv[i]);
        char *epub_file = strcat(strtok(c, "."), ".epub ");
        strcat(zipcommand, epub_file);
    }

    for (int i = 1; i < argc; i++){
        pid_t pid = fork();
        if (pid == 0){
            printf("[pid%d] converting %s ...\n", getpid(), argv[i]);
            char string[10000] = "pandoc ";
            strcat(string, argv[i]);
            strcat(string, " -o ");
            strcat(string, strcat(strtok(argv[i], "."), ".epub"));

            if (execlp("/bin/sh", "/bin/sh", "-c", string, (char *)NULL) == -1)
                perror("execlp():");
            printf("Child %d finished\n", i);
            return 0;
        }
        else if (pid == -1){
            perror("fork():");
            return 0;
        }
    }
    // Waiting for the childs to end
    for (int i = 1; i < argc; i++){
        if (wait(NULL) == -1)
            perror("wait():");
    }
    // executing the zip command
    if (execlp("/bin/sh", "/bin/sh", "-c", zipcommand, (char *)NULL) == -1)
        perror("execlp():");

    return 0;
}
