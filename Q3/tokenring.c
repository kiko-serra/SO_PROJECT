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

// Gets the probability divider, for example:
// If probability is 0.1 we get 10, if its 0.01 we get 100 etc
int getProbabilityDivider(float probability){

    if(probability==1)
        return 1;
    int divider=1;

    // its "<0.99" because 0.1,0.001 and the like dont work otherwise
    while(probability<0.99){
        probability*=(float)10;     
        divider*=10;
    }
    return divider;
}


// Creates the pipes needed
int createpipes(int numberOfPipes)
{
    char *pipe = (char *)malloc(1 * sizeof(char));
    for (int i = 1; i <= numberOfPipes; i++){
        int length1 = snprintf(NULL, 0, "%d", i);
        if (i == numberOfPipes){
            pipe = (char *)realloc(pipe, (length1 + 7 + 1) * sizeof(char));
            sprintf(pipe, "pipe%dto1", i);
        }
        else{
            int length2 = snprintf(NULL, 0, "%d", i + 1);
            pipe = (char *)realloc(pipe, (length1 + length2 + 6 + 1) * sizeof(char));
            sprintf(pipe, "pipe%dto%d", i, i + 1);
        }
        if (mkfifo(pipe, 0777) == -1){
            //It doesn´t throw an error if the file already exists
            if (errno != EEXIST){
                perror("mkfifo():");
                return EXIT_FAILURE;
            }
        }
    }
    free(pipe);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc != 4){
        printf("Usage: tokenring n p t\n");
        return EXIT_FAILURE;
    }
    int numberOfPipes = atoi(argv[1]);
    double probability = atof(argv[2]);
    int waitTime = atoi(argv[3]);
    int randLimit=getProbabilityDivider(probability);
    int val = 0;
    if (createpipes(numberOfPipes)){
        printf("Error creating pipes\n");
        return EXIT_FAILURE;
    }

    pid_t pid[numberOfPipes];
    char *wPipe = (char *)malloc(1 * sizeof(char));
    char *rPipe = (char *)malloc(1 * sizeof(char));
    for (int i = 1; i <= numberOfPipes; i++){
        if ((pid[i - 1] = fork()) < 0){
            fprintf(stderr, "Unable to create pipe: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        else if (pid[i - 1] == 0){
            if (i == numberOfPipes){
                int length1 = snprintf(NULL, 0, "%d", i);
                int length2 = snprintf(NULL, 0, "%d", i - 1);
                wPipe = (char *)realloc(wPipe, (length1 + 7 + 1) * sizeof(char));
                rPipe = (char *)realloc(rPipe, (length1 + length2 + 6 + 1) * sizeof(char));
                sprintf(wPipe, "pipe%dto1", i);
                sprintf(rPipe, "pipe%dto%d", i - 1, i);
            }
            else if (i == 1){
                int length1 = snprintf(NULL, 0, "%d", i);
                int length2 = snprintf(NULL, 0, "%d", i + 1);
                int length3 = snprintf(NULL, 0, "%d", numberOfPipes);
                wPipe = (char *)realloc(wPipe, (length1 + length2 + 6 + 1) * sizeof(char));
                rPipe = (char *)realloc(rPipe, (length3 + 7 + 1) * sizeof(char));
                sprintf(wPipe, "pipe%dto%d", i, i + 1);
                sprintf(rPipe, "pipe%dto1", numberOfPipes);
            }
            else{
                int length1 = snprintf(NULL, 0, "%d", i);
                int length2 = snprintf(NULL, 0, "%d", i + 1);
                int length3 = snprintf(NULL, 0, "%d", i - 1);
                wPipe = (char *)realloc(wPipe, (length1 + length2 + 6 + 1) * sizeof(char));
                rPipe = (char *)realloc(rPipe, (length1 + length3 + 6 + 1) * sizeof(char));
                sprintf(wPipe, "pipe%dto%d", i, i + 1);
                sprintf(rPipe, "pipe%dto%d", i - 1, i);
            }
      
            srand(time(NULL)-i);
            //Array to store the pipes
            int fd[2];

            if (i == 1){
                if ((fd[1] = open(wPipe, O_WRONLY)) < 0){
                    fprintf(stderr, "Unable to read from file: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                val++;
                if (write(fd[1], &val, sizeof(int)) < 0){
                    fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                close(fd[1]);
            }

            while (1){
                //Read value from previous process 
                if ((fd[0] = open(rPipe, O_RDONLY)) < 0){
                    fprintf(stderr, "Unable to read from file: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                if (read(fd[0], &val, sizeof(int)) < 0){
                    fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                close(fd[0]);

                float number=((float)(rand()%randLimit+1)/randLimit);
                
                if (number <= probability){
                    printf("[p%d] lock on token (val = %d)\n", i, val);
                    sleep(waitTime);
                    printf("[p%d] unlock token\n", i);
                }
                val++;
                //Writes value to next process
                if ((fd[1] = open(wPipe, O_WRONLY)) < 0){
                    fprintf(stderr, "Unable to read from file: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                if (write(fd[1], &val, sizeof(int)) < 0){
                    fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                close(fd[1]);
            }
            exit(EXIT_SUCCESS);
        }
    }
    //Let the memory be free!!
    free(wPipe);
    free(rPipe);
    for (int i = 0; i < numberOfPipes; i++)
        if (waitpid(pid[i], NULL, 0) < 0){
            perror("waitpid():");
            return EXIT_FAILURE;
        }

    return EXIT_SUCCESS;
}