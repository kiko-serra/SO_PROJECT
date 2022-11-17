#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//checks if string has "\n" and substitutes it for ' '(space) 
void checkString(char * string){
    for(int i=0; i<strlen(string); i++)
        if(string[i]=='\n')
            string[i]=' ';
}

int main(int argc, char* argv[]){
    if(argc < 3){
        printf("Usage: samples file numberfrags maxfragsize)\n");
        return EXIT_FAILURE;
    }
    //Opens file and checks if file exists
    FILE *fd = fopen(argv[1], "r");
    if(fd == NULL){
        perror("fopen():");
        return EXIT_FAILURE;
    }
    //Converts arguments to integers
    int numberfrags = atoi(argv[2]);
    int maxfragsize = atoi(argv[3]);

    //Gets file size
    if(fseek(fd, 0, SEEK_END) == -1){
        perror("fseek():");
        return EXIT_FAILURE;
    }
    int file_size = ftell(fd);
    
    if(file_size == -1){
        perror("ftell():");
        return EXIT_FAILURE;
    }
    rewind(fd);

    if (maxfragsize > file_size){
        printf("Error: maxfragsize is bigger than file size\n");
        return EXIT_FAILURE;
    }
    
    int maxrandomlimit = file_size - maxfragsize;

    //Functions for random number generation
    time_t t;
    srand((unsigned) time(&t));

    for (int i=0; i < numberfrags; i++){
        int random = rand() % maxrandomlimit;
        char* buf = (char *) malloc(maxfragsize+1*(sizeof(char)));
        if(fseek(fd, random, SEEK_SET)==-1){
            perror("fseek() inside for loop:");
            return EXIT_FAILURE;
        }
        if(fread(buf, 1,maxfragsize, fd)==-1){
            perror("fread() inside for loop:");
            return EXIT_FAILURE;
        }
        buf[maxfragsize] = '\0';
        checkString(buf);
        printf(">%s<\n", buf);
        free(buf);
    }
    if(fclose(fd)==-1){
        perror("fclose():");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
