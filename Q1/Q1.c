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
        printf("Error opening file %s\n", argv[1]);
        return 0;
    }
    //Converts arguments to integers
    int numberfrags = atoi(argv[2]);
    int maxfragsize = atoi(argv[3]);

    //Gets file size
    fseek(fd, 0, SEEK_END);
    int file_size = ftell(fd);
    rewind(fd);
    int maxrandomlimit = file_size - maxfragsize;

    //Functions for random number generation
    time_t t;
    srand((unsigned) time(&t));

    for (int i=0; i < numberfrags; i++){
        int random = rand() % maxrandomlimit;
        char* buf = malloc(maxfragsize+1);
        fseek(fd, random, SEEK_SET);
        fread(buf, maxfragsize, 1, fd);
        buf[maxfragsize] = '\0';
        printf(">%s<\n", buf);
        free(buf);
    }
    fclose(fd);
    return 0;
}
