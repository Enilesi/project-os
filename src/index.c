#include <stdio.h>
#include <stdlib.h> 

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Invalid number of arguments\n");
        return 1; 
    }

    FILE *f = fopen("../data/file.txt", "w");
    if (f == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    int s = atoi(argv[1]) + atoi(argv[2]) + atoi(argv[3]);
    fprintf(f, "%d\n", s);  

    fclose(f);
    return 0;
}
