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

    char* zip_argument_list[argc+2];
    zip_argument_list[0]=(char*)malloc((strlen("zip")+1)*sizeof(char));
    strcpy(zip_argument_list[0],"zip");
    zip_argument_list[1]=(char*)malloc((1+strlen("ebooks.zip"))*sizeof(char));
    strcpy(zip_argument_list[1],"ebooks.zip");
    zip_argument_list[argc+1]=NULL;
  
   
    for (int i = 1; i < argc; i++){
        if(checkExtension(argv[i])){
            printf("Error: %s is not a .txt file\n", argv[i]);
            return EXIT_FAILURE;
        }
        char *c=(char*)malloc(strlen(argv[i])+3);
        strcpy(c, argv[i]);
        getFileName(c);
        char *epub_file = strcat(c,".epub");
        zip_argument_list[1+i]=(char*)malloc((strlen(epub_file)+1)*sizeof(char));
        strcpy(zip_argument_list[1+i], epub_file);
        free(c);
      
    }

    for (int i = 1; i < argc; i++){
        pid_t pid = fork();
        if (pid == 0){
            printf("[pid%d] converting %s ...\n", getpid(), argv[i]);

        
            char* oldfilename=(char*)malloc(strlen(argv[i])+2);
            strcpy(oldfilename,argv[i]);
            char *newfilename=(char*)malloc(strlen(argv[i])+2);
            getFileName(oldfilename);
            
            strcat(newfilename,strcat(oldfilename,".epub"));
            if (execlp("pandoc", "pandoc", argv[i],"-o",newfilename, (char *)NULL) == -1)
                perror("execlp():");

            free(oldfilename);
            free(newfilename);    
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

    sleep(5);
    // Executing the zip command
    for(int i=0;i<argc+2;i++){
       // if(zip_argument_list[i]==NULL)
        printf("arra[%d]='%s'\n",i,zip_argument_list[i]);
    }
    if (execvp("zip",zip_argument_list)== -1){
        perror("execvp():");
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
