#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(int argc, char* argv[])
{
    if(argc < 3){
        printf("Usage: samples file numberfrags maxfragsize)\n");
        return 0;
    }
    //Opens file and checks if file exists
    FILE *fd = fopen(argv[1], "r");
    if(fd == NULL){
        perror("fopen():");
        return 0;
    }
    //Converts arguments to integers
    int numberfrags = atoi(argv[2]);
    int maxfragsize = atoi(argv[3]);

    //Gets file size
    if(fseek(fd, 0, SEEK_END) == -1)
        perror("fseek():");
    int file_size = ftell(fd);
    if(file_size == -1)
        perror("ftell():");
    rewind(fd);
    int maxrandomlimit = file_size - maxfragsize;

    //Functions for random number generation
    time_t t;
    srand((unsigned) time(&t));

    for (int i=0; i < numberfrags; i++){
        int random = rand() % maxrandomlimit;
        char* buf = malloc(maxfragsize+1);
        if(fseek(fd, random, SEEK_SET)==-1)
            perror("fseek() inside for loop:");
        if(fread(buf, maxfragsize, 1, fd)==-1)
            perror("fread() inside for loop:");
        buf[maxfragsize] = '\0';
        printf(">%s<\n", buf);
        free(buf);
    }
    if(fclose(fd)==-1)
        perror("fclose():");
    return 0;
}
