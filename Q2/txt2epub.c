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
        return EXIT_FAILURE;
    }
   // char zipcommand[10000] = "zip ebooks.zip ";
   char * zipcommand=(char*)malloc(16);
   strcpy(zipcommand,"zip ebooks.zip ");
    zipcommand[15]='\0';
    for (int i = 1; i < argc; i++){
        if(checkExtension(argv[i])){
            printf("Error: %s is not a .txt file\n", argv[i]);
            return EXIT_FAILURE;
        }
        printf("TAmanho argv[%d]:%d\n",i,strlen(argv[i]));
        char *c=(char*)malloc(strlen(argv[i])+2);
        strcpy(c, argv[i]);
        getFileName(c);
        printf("C:%s\n",c);
        char *epub_file = strcat(c, ".epub ");
        printf("Epub:%s\n",epub_file);
        zipcommand=(char *)realloc(zipcommand,strlen(zipcommand)+strlen(epub_file)+1);
        strcat(zipcommand, epub_file);
        zipcommand[strlen(zipcommand)]='\0'; 
        
        free(c);
        //free(epub_file);
    }

    for (int i = 1; i < argc; i++){
        pid_t pid = fork();
        if (pid == 0){
            printf("[pid%d] converting %s ...\n", getpid(), argv[i]);

            //Creating command to use in execlp
          //  char command[10000] = "pandoc ";
            char * command=(char *)malloc(8);
            strcpy(command,"pandoc ");
            command=(char*)realloc(command,strlen(command)+strlen(argv[i]));
            strcat(command, argv[i]);
            command=(char*)realloc(command,strlen(command)+5);
            strcat(command, " -o ");
            
            //!!! mudar strcat para variaveis e fazer strlen +2 
            getFileName(argv[i]);
            command=(char*)realloc(command,strlen(command)+strlen(argv[i])+6);
            strcat(command, strcat(argv[i], ".epub"));
            command[strlen(command)]='\0';
            if (execlp("/bin/sh", "/bin/sh", "-c", command, (char *)NULL) == -1)
                perror("execlp():");

            free(command);    
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
    if (execlp("/bin/sh", "/bin/sh", "-c", zipcommand, (char *)NULL) == -1){
        perror("execlp():");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
