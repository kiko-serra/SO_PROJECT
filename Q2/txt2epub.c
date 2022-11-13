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
    for (int i = 1; i < argc; i++){
        pid_t pid = fork();
        if (pid == 0){
            if (execlp("pandoc", "pandoc", argv[i], "-o", strcat(strtok(argv[i], "."), ".epub"), NULL) == -1)
                perror("execlp():");
            printf("Child %d finished\n", i);
            return 0;
        }
        else if (pid == -1){
            perror("fork():");
            return 0;
        }
    }
    for (int i = 1; i < argc; i++){
        if (wait(NULL) == -1)
            perror("wait():");
    }
    return 0;
}
