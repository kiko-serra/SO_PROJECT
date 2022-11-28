#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Checks if string has "\n" and substitutes it for ' ' (space)
void checkString(char *string){
    for (int i = 0; i < strlen(string); i++)
        if (string[i] == '\n')
            string[i] = ' ';
}

int main(int argc, char *argv[]){
    if (argc < 3){
        printf("Usage: samples file numberFrags maxFragSize)\n");
        return EXIT_FAILURE;
    }
    // Opens file and checks if file exists
    FILE *fd = fopen(argv[1], "r");
    if (fd == NULL){
        perror("fopen():");
        return EXIT_FAILURE;
    }
    // Converts arguments to integers
    int numberFrags = atoi(argv[2]);
    int maxFragSize = atoi(argv[3]);

    // Gets file size
    if (fseek(fd, 0, SEEK_END) == -1){
        perror("fseek():");
        return EXIT_FAILURE;
    }
    int fileSize = ftell(fd);

    if (fileSize == -1){
        perror("ftell():");
        return EXIT_FAILURE;
    }
    rewind(fd);

    if (maxFragSize > fileSize){
        printf("Error: maxfragsize is bigger than file size\n");
        return EXIT_FAILURE;
    }
    int maxRandomLimit = fileSize - maxFragSize;

    // Functions for random number generation
    time_t t;
    srand((unsigned)time(&t));

    for (int i = 0; i < numberFrags; i++){
        int random = rand() % maxRandomLimit;
        char *buf = (char *)malloc((maxFragSize + 1) * sizeof(char));
        if (fseek(fd, random, SEEK_SET) == -1){
            perror("fseek() inside for loop:");
            return EXIT_FAILURE;
        }
        if (fread(buf, 1, maxFragSize, fd) == -1){
            perror("fread() inside for loop:");
            return EXIT_FAILURE;
        }
        buf[maxFragSize] = '\0';
        checkString(buf);
        printf(">%s<\n", buf);
        free(buf);
    }
    if (fclose(fd) == -1){
        perror("fclose():");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
