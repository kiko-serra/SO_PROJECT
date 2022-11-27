#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <fcntl.h>



//creates a named pipe, example: pipe1to2
int createpipes(int numberOfPipes){
    
    char * pipe=(char*)malloc(1*sizeof(char));
  
    for (int i = 1; i <= numberOfPipes; i++)
    {   
        int length1= snprintf( NULL, 0, "%d", i );
        if(i==numberOfPipes){
            pipe=(char*)realloc(pipe,(length1+7+1)*sizeof(char));
            sprintf(pipe, "pipe%dto1", i);
        }
        else{
            int length2=snprintf( NULL, 0, "%d", i+1 );
            pipe=(char*) realloc(pipe,(length1+length2+6+1)*sizeof(char));    
            sprintf(pipe, "pipe%dto%d", i,i+1);
        }
        if(mkfifo(pipe,0777)==-1){ 
            //it doesn´t trow an error if the file already exists
            if(errno !=EEXIST){
                perror("mkfifo():");
                return EXIT_FAILURE;
            }
        } 
    }
    free(pipe);
    return 0;
/*
     char * pipe=(char*)malloc(5);
    strcpy(pipe,"pipe");

    // turns the number from into  a string
    int length = snprintf( NULL, 0, "%d", from );
    char* fromnumber = malloc( length + 1 );
    snprintf( fromnumber, length + 1, "%d", from );
    fromnumber[length]='\0';
    
    // turns the number to into  a string
    length = snprintf( NULL, 0, "%d", to );
    char * tonumber= malloc( length + 1 );
    snprintf( tonumber, length + 1, "%d", to );
    tonumber[length]='\0';
    
   
    pipe=(char*)realloc(pipe,strlen(pipe)+strlen(fromnumber)+2+strlen(tonumber)+1);
    strcat(pipe,fromnumber);
    strcat(pipe,"to");
    strcat(pipe,tonumber);  
    pipe[strlen(pipe)]='\0';
    


    if(mkfifo(pipe,0777)==-1){ 
        // só da erro caso o ficheiro não exista
        if(errno !=EEXIST){
            perror("mkfifo():");
            return EXIT_FAILURE;
        }   
    }

    free(fromnumber);
    free(tonumber);
    free(pipe);

    return 0;
    */
}


int main(int argc, char* argv[]){

    if(argc != 4){
        printf("Usage: tokenring n p t\n");
        return EXIT_FAILURE;
    }

    int numberOfPipes=atoi(argv[1]);
    double probability=atof(argv[2]);
    int waitTime=atoi(argv[3]);
    int val=0;
    time_t t;
    //srand((unsigned) time(&t));
   
    if(createpipes(numberOfPipes)){
        printf("Error creating pipes\n");
        return 1;
    }
    
    pid_t pid[numberOfPipes];
    char *wpipe = (char*)malloc(1 * sizeof(char));
    char *rpipe = (char*)malloc(1 * sizeof(char));
    for (int i = 1; i <= numberOfPipes; i++)
    {
        if((pid[i-1]=fork())<0){
            fprintf(stderr, "Unable to create pipe: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        else if(pid[i-1] == 0){
            if(i == atoi(argv[1])) {
                int length1= snprintf( NULL, 0, "%d", i );
                int length2= snprintf( NULL, 0, "%d", i-1 );
                wpipe=(char*)realloc(wpipe,(length1+7+1)*sizeof(char));
                rpipe=(char*)realloc(rpipe,(length1+length2+6+1)*sizeof(char));
                sprintf(wpipe, "pipe%dto1", i);
                sprintf(rpipe, "pipe%dto%d", i-1, i);
            } 
            else if(i == 1) {
                int length1= snprintf( NULL, 0, "%d", i );
                int length2= snprintf( NULL, 0, "%d", i+1 );
                int length3=snprintf( NULL, 0, "%d", numberOfPipes);
                wpipe=(char*)realloc(wpipe,(length1+length2+6+1)*sizeof(char));
                rpipe=(char*)realloc(rpipe,(length3+7+1)*sizeof(char));
                sprintf(wpipe, "pipe%dto%d", i, i+1);
                sprintf(rpipe, "pipe%dto1", numberOfPipes);
            } 
            else {
                int length1= snprintf( NULL, 0, "%d", i );
                int length2= snprintf( NULL, 0, "%d", i+1 );
                int length3=snprintf( NULL, 0, "%d", i-1);
                wpipe=(char*)realloc(wpipe,(length1+length2+6+1)*sizeof(char));
                rpipe=(char*)realloc(rpipe,(length1+length3+6+1)*sizeof(char));
                sprintf(wpipe, "pipe%dto%d", i, i+1);
                sprintf(rpipe, "pipe%dto%d", i-1, i);
            }
            srandom(time(NULL) - i );
            /* store pipes in an array */
            int fd[2];

            if(i == 1) {
                if((fd[1] = open(wpipe, O_WRONLY)) < 0) {
                    fprintf(stderr, "Unable to read from file: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                val++;

                if(write(fd[1], &val, sizeof(int)) < 0) {
                     fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
                     exit(EXIT_FAILURE);
                }

                close(fd[1]);
            }


            while(1) {

                /* read value from previous process */
                if((fd[0] = open(rpipe, O_RDONLY)) < 0) {
                    fprintf(stderr, "Unable to read from file: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                if(read(fd[0], &val, sizeof(int)) < 0) {
                    fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                close(fd[0]);

                val++; // increments value
                
                int rand = random() % (int)( 1/probability);
                if(rand == 1) {
                    printf("[p%d] lock on token (val = %d)\n", i, val);
                    sleep(waitTime);
                    printf("[p%d] unlock token\n", i);
                }

                /* writes value to next process */
                if((fd[1] = open(wpipe, O_WRONLY)) < 0) {
                    fprintf(stderr, "Unable to read from file: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                
                if(write(fd[1], &val, sizeof(int)) < 0) {
                    fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                close(fd[1]);
            }

             exit(EXIT_SUCCESS);
        }
    }

     for(int i = 0; i < numberOfPipes; i++) {
        if(waitpid(pid[i], NULL, 0) < 0) {
            fprintf(stderr, "%s: waitpid error: %s\n", argv[0], strerror(errno));
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
    /*
     pid_t pid = fork();
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        // parent 
       
        printf("No pai:\n");

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
    /*    if ( waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
        }
        
        exit(EXIT_SUCCESS);
    }
    else {
        
         printf("No filho:\n");       
         int fd1=open("pipe1to2",O_RDONLY); //returns file descriptor
         int fd2=open("pipe2to1",O_WRONLY); //returns file descriptor
         char line[256];
         int nbytes;
        while(1){
            if ((nbytes = read(fd1, line, 256)) <=0 ) {   
                if(errno !=EEXIST){
                    
                }
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


    return 0;*/
}