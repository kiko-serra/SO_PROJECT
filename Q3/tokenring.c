#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> // as ultimas 3 são para o mkfifo
#include <fcntl.h>

void increasenumber(char*buf){

    int valor=atoi(buf);
    valor++;
    sprintf(buf, "%d", valor);
}
int main(int argc, char* argv[]){

    if(argc != 4){
        printf("Usage: tokenring n p t\n");
        return EXIT_FAILURE;
    }

    int numberOfPipes=atoi(argv[1]);
    double probability=atof(argv[2]);
    int waitTime=atoi(argv[3]);
    time_t t;
    srand((unsigned) time(&t));

   
 // -------------------- CREATE PIPES----------------------------------------   
    if(mkfifo("pipe1to2",0777)==-1){ 
        // só da erro caso o ficheiro não exista
        if(errno !=EEXIST){
            perror("mkfifo():");
            return EXIT_FAILURE;
        }   
    }
    if(mkfifo("pipe2to1",0777)==-1){ 
        // só da erro caso o ficheiro não exista
        if(errno !=EEXIST){
            perror("mkfifo():");
            return EXIT_FAILURE;
        }   
    }
// ------------------------------------------------------------
    pid_t pid;
    if ((pid = fork()) < 0) {
    perror("fork error");
    exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        /* parent */
       
        printf("No pai:\n");
//--------------------- OPENING PIPES-----------------------------------------
        int fd1=open("pipe1to2",O_WRONLY); //returns file descriptor
        if(fd1==-1){
        printf("Error openning the file\n");
        return EXIT_FAILURE;
        }
        int fd2=open("pipe2to1",O_RDONLY); //returns file descriptor
        if(fd2==-1){
        printf("Error openning the file 2\n");
        return EXIT_FAILURE;
        }
// ------------------------------------------------------------

        char line[256]="100";
        int nbytes;

 // ---------------CICLO ESCREVER E LER-------------------------------------------       
    while(1){

        if(write(fd1,line,sizeof(line))==-1){
            perror("write()");
            exit(EXIT_FAILURE);
        }


        if ((nbytes = read(fd2, line, 256)) <=0 ) {
             if(errno !=EEXIST)
                fprintf(stderr, "Unable to read from file: %s\n", strerror(errno));
                close(fd2);
        }
        else{
            
            write(STDOUT_FILENO, line, nbytes);
            printf("\n");
            increasenumber(line);
        }
    }
        
 // ------------------------------------------------------------        
        if ( waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
        }
        exit(EXIT_SUCCESS);
    }
    else {
        
         printf("No filho:\n");
 //--------------------- OPENING PIPES-----------------------------------------        
         int fd1=open("pipe1to2",O_RDONLY); //returns file descriptor
         int fd2=open("pipe2to1",O_WRONLY); //returns file descriptor
 // ------------------------------------------------------------ 

         char line[256];
         int nbytes;

//--------------------- CICLO LER E ESCREVER-----------------------------------------
        while(1){

            if ((nbytes = read(fd1, line, 256)) <=0 ) {   
                if(errno !=EEXIST)
                    fprintf(stderr, "Unable to read from file: %s\n", strerror(errno));
                    close(fd1);
                }
            else{
                write(STDOUT_FILENO, line, nbytes);
                printf("\n");
            }
                    
            
            increasenumber(line);
        
            if(write(fd2,line,sizeof(line))==-1){
                perror("write()");
                exit(EXIT_FAILURE);
            }

        }
         
         
        exit(EXIT_SUCCESS);
    }
    /*printf("Opening...\n");
    int fd=open("myfifo1",O_WRONLY); //returns file descriptor
    if(fd==-1){
        printf("Error openning the file\n");
        return EXIT_FAILURE;
    }
    printf("Oppened..\n");
    int x = 97;
    if(write(fd,&x,sizeof(x))==-1)
        perror("write()");
        return EXIT_FAILURE;
    ;
    printf("Written\n");
    close(fd);
    printf("Clossed\n");
*/ 

    return 0;
}