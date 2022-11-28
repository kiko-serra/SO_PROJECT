#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <time.h>



//creates the pipes needed
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
    char *wPipe = (char*)malloc(1 * sizeof(char));
    char *rPipe = (char*)malloc(1 * sizeof(char));
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
                wPipe=(char*)realloc(wPipe,(length1+7+1)*sizeof(char));
                rPipe=(char*)realloc(rPipe,(length1+length2+6+1)*sizeof(char));
                sprintf(wPipe, "pipe%dto1", i);
                sprintf(rPipe, "pipe%dto%d", i-1, i);
            } 
            else if(i == 1) {
                int length1= snprintf( NULL, 0, "%d", i );
                int length2= snprintf( NULL, 0, "%d", i+1 );
                int length3=snprintf( NULL, 0, "%d", numberOfPipes);
                wPipe=(char*)realloc(wPipe,(length1+length2+6+1)*sizeof(char));
                rPipe=(char*)realloc(rPipe,(length3+7+1)*sizeof(char));
                sprintf(wPipe, "pipe%dto%d", i, i+1);
                sprintf(rPipe, "pipe%dto1", numberOfPipes);
            } 
            else {
                int length1= snprintf( NULL, 0, "%d", i );
                int length2= snprintf( NULL, 0, "%d", i+1 );
                int length3=snprintf( NULL, 0, "%d", i-1);
                wPipe=(char*)realloc(wPipe,(length1+length2+6+1)*sizeof(char));
                rPipe=(char*)realloc(rPipe,(length1+length3+6+1)*sizeof(char));
                sprintf(wPipe, "pipe%dto%d", i, i+1);
                sprintf(rPipe, "pipe%dto%d", i-1, i);
            }
            srandom(time(NULL) - i );

            //array to store the pipes
            int fd[2];

            if(i == 1) {
                if((fd[1] = open(wPipe, O_WRONLY)) < 0) {
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
                if((fd[0] = open(rPipe, O_RDONLY)) < 0) {
                    fprintf(stderr, "Unable to read from file: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                if(read(fd[0], &val, sizeof(int)) < 0) {
                    fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                close(fd[0]);

              
                
                int number = rand() % (int)( 1/probability);
                if(number == 0) {
                    printf("[p%d] lock on token (val = %d)\n", i, val);
                    sleep(waitTime);
                    printf("[p%d] unlock token\n", i);
                }

                val++;

                /* writes value to next process */
                if((fd[1] = open(wPipe, O_WRONLY)) < 0) {
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

    free(wPipe);
    free(rPipe);
     for(int i = 0; i < numberOfPipes; i++) {
        if(waitpid(pid[i], NULL, 0) < 0) {
            fprintf(stderr, "%s: waitpid error: %s\n", argv[0], strerror(errno));
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
   
}